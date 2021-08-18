#include "LevelManager.h"
#include "Theme.h"

#include "ShapeManager.h"
#include "../rendering/Mesh.h"
#include "../rendering/MeshRenderer.h"

LevelManager* LevelManager::inst;

LevelManager::LevelManager()
{
	if (inst)
	{
		return;
	}

	inst = this;

	ColorSlot::init();

	// Initialize startup level
	{
		level = new Level();

		level->name = "Untitled level";
		level->song = R"(C:\Users\UserTCQ\Music\Creo-Dimension.mp3)";

		audioClip = new AudioClip(level->song.c_str());

		// Intialize 30 color slots
		for (int i = 0; i < 30; i++)
		{
			ColorKeyframe kf = ColorKeyframe();
			kf.time = 0.0f;
			kf.color = Color(1.0f, 1.0f, 1.0f);

			ColorSlot* colorSlot = new ColorSlot(1, &kf);

			level->colorSlots.push_back(colorSlot);
		}

		// Intialize startup object
		LevelObject* obj = new LevelObject();
		obj->name = "Hello world!";
		obj->startTime = 1.0f;
		obj->killTime = 5.0f;

		Keyframe keyframes[2]
		{
			Keyframe(0.0f, 0.0f),
			Keyframe(4.0f, 5.0f)
		};

		AnimationChannel* channel = new AnimationChannel(AnimationChannelType_PositionX, 2, keyframes);
		obj->insertChannel(channel);

		insertObject(obj);
	}

	timeline = new Timeline();
	properties = new Properties();
	theme = new Theme();
}

void LevelManager::loadLevel(nlohmann::json j)
{
	// Cleanup old level
	if (level)
	{
		aliveObjects.clear();
		objectActions.clear();
		delete audioClip;
		delete level;
	}

	// Parse new level json
	Level* newLevel = new Level();
	newLevel->name = j["name"].get<std::string>();
	newLevel->song = j["song"].get<std::string>();

	level = newLevel;

	nlohmann::json::array_t slotArr = j["color_slots"].get<nlohmann::json::array_t>();
	for (nlohmann::json slotJson : slotArr)
	{
		newLevel->colorSlots.push_back(new ColorSlot(slotJson));
	}

	nlohmann::json::array_t objArr = j["objects"].get<nlohmann::json::array_t>();
	for (nlohmann::json objJson : objArr)
	{
		recursivelyInitializeObjectTree(objJson, nullptr, newLevel);
	}

	recalculateAllObjectActions();
	recalculateActionIndex(time);
	updateLevel(0.0f);

	audioClip = new AudioClip(newLevel->song.c_str());
}

void LevelManager::update()
{
	if (audioClip->isPlaying())
	{
		updateLevel(audioClip->getPosition());
	}
}

void LevelManager::updateLevel(float time)
{
	this->time = time;

	if (time > lastTime)
	{
		while (actionIndex < objectActions.size() && objectActions[actionIndex].time < time)
		{
			switch (objectActions[actionIndex].type)
			{
			case ObjectActionType_Spawn:
				objectActions[actionIndex].levelObject->node->setActive(true);
				aliveObjects.insert(objectActions[actionIndex].levelObject);
				break;
			case ObjectActionType_Kill:
				objectActions[actionIndex].levelObject->node->setActive(false);
				aliveObjects.erase(objectActions[actionIndex].levelObject);
				break;
			}
			actionIndex++;
		}
	}
	else
	{
		while (actionIndex > 0 && objectActions[actionIndex - 1].time > time)
		{
			switch (objectActions[actionIndex - 1].type)
			{
			case ObjectActionType_Spawn:
				objectActions[actionIndex - 1].levelObject->node->setActive(false);
				aliveObjects.erase(objectActions[actionIndex - 1].levelObject);
				break;
			case ObjectActionType_Kill:
				objectActions[actionIndex - 1].levelObject->node->setActive(true);
				aliveObjects.insert(objectActions[actionIndex - 1].levelObject);
				break;
			}
			actionIndex--;
		}
	}

	lastTime = time;

	// Animate alive objects
	for (LevelObject* levelObject : aliveObjects)
	{
		updateObject(levelObject);
	}

	// Update all color slots
	for (ColorSlot* colorSlot : level->colorSlots)
	{
		updateColorSlot(colorSlot);
	}
}

void LevelManager::updateObject(LevelObject* levelObject)
{
	for (AnimationChannel* channel : levelObject->animationChannels)
	{
		switch (channel->type)
		{
		case AnimationChannelType_PositionX:
			levelObject->node->transform->position.x = channel->update(time - levelObject->startTime);
			break;
		case AnimationChannelType_PositionY:
			levelObject->node->transform->position.y = channel->update(time - levelObject->startTime);
			break;
		case AnimationChannelType_ScaleX:
			levelObject->node->transform->scale.x = channel->update(time - levelObject->startTime);
			break;
		case AnimationChannelType_ScaleY:
			levelObject->node->transform->scale.y = channel->update(time - levelObject->startTime);
			break;
		case AnimationChannelType_Rotation:
			levelObject->node->transform->rotation = glm::angleAxis(
				channel->update(time - levelObject->startTime) / 180.0f * 3.14159265359f,
				glm::vec3(0.0f, 0.0f, -1.0f));
			break;
		}
	}

	levelObject->node->transform->position.z = levelObject->depth;
}

void LevelManager::updateColorSlot(ColorSlot* colorSlot)
{
	colorSlot->update(time);
}

void LevelManager::recalculateAllObjectActions()
{
	// Clear and re-add object actions
	objectActions.clear();

	for (int i = 0; i < level->levelObjects.size(); i++)
	{
		ObjectAction spawnAction, killAction;
		level->levelObjects[i]->genActionPair(&spawnAction, &killAction);

		insertAction(spawnAction);
		insertAction(killAction);
	}
}

void LevelManager::recalculateObjectAction(LevelObject* levelObject)
{
	// Find and remove old actions
	std::vector<ObjectAction>::iterator it = std::remove_if(objectActions.begin(), objectActions.end(),
	                                                        [levelObject](ObjectAction match)
	                                                        {
		                                                        return match.levelObject == levelObject;
	                                                        });
	objectActions.erase(it, objectActions.end());

	// Create new actions and insert
	ObjectAction spawnAction, killAction;
	levelObject->genActionPair(&spawnAction, &killAction);

	insertAction(spawnAction);
	insertAction(killAction);
}

void LevelManager::recalculateActionIndex(float time)
{
	// Reset all objects
	for (LevelObject* levelObject : level->levelObjects)
	{
		levelObject->node->setActive(false);
	}

	// Reset action index and recalculate
	actionIndex = 0;
	while (actionIndex < objectActions.size() && objectActions[actionIndex].time < time)
	{
		switch (objectActions[actionIndex].type)
		{
		case ObjectActionType_Spawn:
			objectActions[actionIndex].levelObject->node->setActive(true);
			aliveObjects.insert(objectActions[actionIndex].levelObject);
			break;
		case ObjectActionType_Kill:
			objectActions[actionIndex].levelObject->node->setActive(false);
			aliveObjects.erase(objectActions[actionIndex].levelObject);
			break;
		}
		actionIndex++;
	}
}

void LevelManager::insertObject(LevelObject* levelObject)
{
	ObjectAction spawn, kill;
	levelObject->genActionPair(&spawn, &kill);

	insertAction(spawn);
	insertAction(kill);

	spawnObject(levelObject);
	level->levelObjects.push_back(levelObject);

	recalculateActionIndex(time);
}

void LevelManager::removeObject(LevelObject* levelObject)
{
	// Remove object
	std::vector<LevelObject*>::iterator objIt = std::remove(level->levelObjects.begin(), level->levelObjects.end(), levelObject);
	level->levelObjects.erase(objIt);

	// Remove actions
	std::vector<ObjectAction>::iterator it = std::remove_if(objectActions.begin(), objectActions.end(),
		[levelObject](ObjectAction match)
		{
			return match.levelObject == levelObject;
		});
	objectActions.erase(it, objectActions.end());

	// Remove from alive objects
	aliveObjects.erase(levelObject);

	// Delete object
	delete levelObject;

	recalculateActionIndex(time);
}

void LevelManager::insertAction(ObjectAction value)
{
	if (objectActions.size() == 0)
	{
		objectActions.push_back(value);
		return;
	}

	std::vector<ObjectAction>::iterator it = std::lower_bound(objectActions.begin(), objectActions.end(), value,
	                                                          [](ObjectAction a, ObjectAction b)
	                                                          {
		                                                          return a.time < b.time;
	                                                          });
	objectActions.insert(it, value);
}

void LevelManager::spawnObject(LevelObject* levelObject)
{
	SceneNode* node = new SceneNode(levelObject->name);
	node->setActive(false);

	MeshRenderer* renderer = new MeshRenderer();
	renderer->mesh = ShapeManager::inst->shapes[levelObject->shapeIndex].mesh;
	renderer->material = level->colorSlots[levelObject->colorSlotIndex]->material;

	node->renderer = renderer;

	levelObject->node = node;
}

void LevelManager::recursivelyInitializeObjectTree(nlohmann::json j, LevelObject* parent, Level* level)
{
	LevelObject* obj = new LevelObject(j);

	level->levelObjects.push_back(obj);
	spawnObject(obj);

	obj->setParent(parent);

	if (j.contains("children")) 
	{
		nlohmann::json::array_t children = j["children"].get<nlohmann::json::array_t>();
		for (nlohmann::json child : children)
		{
			recursivelyInitializeObjectTree(child, obj, level);
		}
	}
}
