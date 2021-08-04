#include "LevelManager.h"

Mesh* mesh;
Shader* shader;
Material* material;

LevelManager::LevelManager()
{
	glm::vec3 vertices[] = {
		glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f)
	};

	uint32_t indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	mesh = new Mesh(4, vertices, 6, indices);
	shader = new Shader("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
	material = new Material(shader, 0, nullptr);

	float start = 5.0f;
	float end = 25.0f;

	{
		LevelObject* obj = new LevelObject("Hello world!");
		obj->startTime = start;
		obj->killTime = end;
		obj->editorBinIndex = 0;

		spawnNode(obj);

		levelObjects.push_back(obj);
	}

	timeline = new Timeline(this);
	properties = new Properties(this);

	recalculateAllObjectActions();
}

void LevelManager::update(float time)
{
	this->time = time;

	// Do not process if paused
	if (time > lastTime)
	{
		while (actionIndex < objectActions.size() && objectActions[actionIndex].time <= time)
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
	else if (time < lastTime)
	{
		while (actionIndex >= 0 && objectActions[actionIndex].time >= time)
		{
			switch (objectActions[actionIndex].type)
			{
			case ObjectActionType_Spawn:
				objectActions[actionIndex].levelObject->node->setActive(false);
				aliveObjects.erase(objectActions[actionIndex].levelObject);
				break;
			case ObjectActionType_Kill:
				objectActions[actionIndex].levelObject->node->setActive(true);
				aliveObjects.insert(objectActions[actionIndex].levelObject);
				break;
			}
			actionIndex--;
		}
	}

	// Animate alive objects
	if (lastTime != time)
	{
		for (LevelObject* levelObject : aliveObjects)
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
		}
	}

	lastTime = time;
}

void LevelManager::recalculateAllObjectActions()
{
	// Clear and re-add object actions
	objectActions.clear();

	for (int i = 0; i < levelObjects.size(); i++)
	{
		ObjectAction spawnAction, killAction;
		levelObjects[i]->genActionPair(&spawnAction, &killAction);

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
	for (LevelObject* levelObject : levelObjects)
	{
		levelObject->node->setActive(false);
	}

	// Reset action index and recalculate
	actionIndex = 0;
	while (actionIndex < objectActions.size() && objectActions[actionIndex].time <= time)
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

void LevelManager::spawnNode(LevelObject* levelObject)
{
	SceneNode* node = new SceneNode(levelObject->name);
	node->setActive(false);

	MeshRenderer* renderer = new MeshRenderer();
	renderer->mesh = mesh;
	renderer->material = material;

	node->renderer = renderer;

	levelObject->node = node;
}
