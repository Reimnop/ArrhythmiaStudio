#include "LevelManager.h"
#include "Theme.h"

#include "ShapeManager.h"
#include "DiscordManager.h"
#include "../rendering/Renderer.h"
#include "../rendering/MeshRenderer.h"

#include <filesystem>
#include <fstream>
#include <ctime>
#include <logger.h>

LevelManager* LevelManager::inst;

LevelManager::LevelManager()
{
	if (inst)
	{
		return;
	}

	inst = this;

	ColorSlot::init();

	timeline = new Timeline();
	properties = new Properties();
	theme = new Theme();
	events = new Events();

	loadLevel("Assets/StartupLevel");
}

void LevelManager::loadLevel(const std::string& levelPath)
{
	// Cleanup old level
	if (level)
	{
		aliveObjects.clear();
		objectActions.clear();
		delete audioClip;
		delete level;
	}

	Logger::info("Loading level at " + levelPath);

	// Parse new level json
	std::filesystem::path levelDir = levelPath;
	std::filesystem::path levelFilePath = levelDir / "level.njelv";
	std::filesystem::path songFilePath = levelDir / "song.ogg";

	std::ifstream s(levelFilePath);
	nlohmann::json j;
	s >> j;
	s.close();

	Level* newLevel = new Level();
	newLevel->name = j["name"].get<std::string>();

	level = newLevel;

	if (j.contains("color_slots"))
	{
		nlohmann::json::array_t slotArr = j["color_slots"].get<nlohmann::json::array_t>();
		for (const nlohmann::json& slotJson : slotArr)
		{
			newLevel->colorSlots.push_back(new ColorSlot(slotJson));
		}
	}

	Logger::info("Loaded " + std::to_string(newLevel->colorSlots.size()) + " color slots");

	if (j.contains("level_events"))
	{
		nlohmann::json::array_t eventArr = j["level_events"].get<nlohmann::json::array_t>();
		for (const nlohmann::json& eventJson : eventArr)
		{
			newLevel->insertLevelEvent(new LevelEvent(eventJson));
		}
	}

	Logger::info("Loaded " + std::to_string(newLevel->levelEvents.size()) + " level events");

	if (j.contains("objects"))
	{
		nlohmann::json::array_t objArr = j["objects"].get<nlohmann::json::array_t>();
		for (const nlohmann::json& objJson : objArr)
		{
			insertObject(new LevelObject(objJson));
		}

		for (const std::pair<uint64_t, LevelObject*> x : newLevel->levelObjects)
		{
			initializeObjectParent(x.second);
		}
	}

	Logger::info("Loaded " + std::to_string(newLevel->levelObjects.size()) + " objects");

	actionIndex = 0;
	time = 0.0f;

	recalculateActionIndex(0.0f);
	updateLevel(0.0f);

	audioClip = new AudioClip(songFilePath.generic_string());

	selectedObjects.clear();
	theme->reset();
	events->reset();
	timeline->genBuffer(audioClip);

	std::string stateStr = "Editing " + newLevel->name;

	discord::Activity activity = discord::Activity();
	activity.SetState(stateStr.c_str());
	activity.GetTimestamps().SetStart(std::time(nullptr));

	DiscordManager::inst->updateActivity(activity);

	Logger::info("Finished loading level " + newLevel->name);
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
		while (indexAdvance(time))
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
		while (indexReverse(time))
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

	// Update all color slots
	for (ColorSlot* colorSlot : level->colorSlots)
	{
		updateColorSlot(colorSlot);
	}

	// Update all level events
	for (LevelEvent* levelEvent : level->levelEvents)
	{
		updateLevelEvent(levelEvent);
	}

	// Update alive objects
	for (LevelObject* levelObject : aliveObjects)
	{
		updateObject(levelObject);
	}
}

void LevelManager::updateColorSlot(ColorSlot* colorSlot)
{
	colorSlot->update(time);
}

void LevelManager::updateLevelEvent(LevelEvent* levelEvent)
{
	Camera* camera = Renderer::inst->camera;

	switch (levelEvent->type)
	{
	case LevelEventType_CameraPositionX:
		camera->position.x = levelEvent->update(time);
		break;
	case LevelEventType_CameraPositionY:
		camera->position.y = levelEvent->update(time);
		break;
	case LevelEventType_CameraScale:
		camera->orthographicScale = levelEvent->update(time);
		break;
	case LevelEventType_CameraRotation:
		camera->rotation = glm::angleAxis(
			levelEvent->update(time) / 180.0f * 3.14159265359f,
			glm::vec3(0.0f, 0.0f, -1.0f));
		break;
	case LevelEventType_BloomIntensity:
		Renderer::inst->bloom->intensity = levelEvent->update(time);
		break;
	case LevelEventType_BloomScatter:
		Renderer::inst->bloom->scatter = levelEvent->update(time);
		break;
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
		case AnimationChannelType_Opacity:
			MeshRenderer* mr = (MeshRenderer*)levelObject->node->renderer;
			mr->opacity = std::clamp(channel->update(time - levelObject->startTime), 0.0f, 1.0f);
			break;
		}
	}

	levelObject->node->transform->position.z = levelObject->depth;
}

void LevelManager::recalculateAllObjectActions()
{
	// Clear and re-add object actions
	objectActions.clear();

	for (const std::pair<uint64_t, LevelObject*> x : level->levelObjects)
	{
		ObjectAction spawnAction, killAction;
		x.second->genActionPair(&spawnAction, &killAction);

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
	for (const std::pair<uint64_t, LevelObject*> x : level->levelObjects)
	{
		x.second->node->setActive(false);
	}

	// Reset action index and recalculate
	actionIndex = 0;
	while (indexAdvance(time))
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
	level->levelObjects.emplace(levelObject->id, levelObject);
}

void LevelManager::removeObject(LevelObject* levelObject)
{
	// Remove actions
	std::vector<ObjectAction>::iterator it = std::remove_if(objectActions.begin(), objectActions.end(),
	                                                        [levelObject](ObjectAction match)
	                                                        {
		                                                        return match.levelObject == levelObject;
	                                                        });
	objectActions.erase(it, objectActions.end());

	// Remove from alive objects
	aliveObjects.erase(levelObject);

	// We cache the id as it will not be available after deleting the object
	const uint64_t id = levelObject->id;

	// Delete object
	delete levelObject;

	// Remove object
	level->levelObjects.erase(id);

	recalculateActionIndex(time);
}

bool LevelManager::initializeObjectParent(LevelObject* levelObject)
{
	if (levelObject->parentId && level->levelObjects.count(levelObject->parentId))
	{
		LevelObject* parent = level->levelObjects[levelObject->parentId];

		levelObject->node->setParent(parent->node);
		parent->childrenId.emplace(levelObject->id);

		return true;
	}

	return false;
}

bool LevelManager::indexAdvance(float time) const
{
	if (actionIndex >= objectActions.size())
		return false;

	if (time >= objectActions[actionIndex].time)
		return true;

	return false;
}

bool LevelManager::indexReverse(float time) const
{
	if (actionIndex <= 0)
		return false;

	if (time < objectActions[actionIndex - 1].time)
		return true;

	return false;
}

void LevelManager::insertAction(ObjectAction value)
{
	if (objectActions.empty())
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

void LevelManager::spawnObject(LevelObject* levelObject) const
{
	SceneNode* node = new SceneNode(levelObject->name);
	node->setActive(false);

	MeshRenderer* renderer = new MeshRenderer();
	renderer->mesh = ShapeManager::inst->shapes[levelObject->shapeIndex].mesh;
	renderer->material = level->colorSlots[levelObject->colorSlotIndex]->material;

	node->renderer = renderer;

	levelObject->node = node;
}
