#include "AddObjectCmd.h"
#include "../LevelManager.h"

#include <assert.h>

AddObjectCmd::AddObjectCmd(LevelObject* obj)
{
	data = obj->toJson();
	children.reserve(obj->childrenId.size());
	for (const uint64_t id : obj->childrenId)
	{
		children.push_back(id);
	}
}

void AddObjectCmd::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;

	levelManager->removeObject(level->levelObjects[data["id"].get<uint64_t>()]);
	levelManager->recalculateActionIndex(levelManager->time);
}

void AddObjectCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;

	LevelObject* obj = new LevelObject(data);

	levelManager->insertObject(obj);
	levelManager->initializeObjectParent(obj);
	levelManager->recalculateActionIndex(levelManager->time);

	levelManager->updateObject(obj);

	for (const uint64_t id : children)
	{
		if (level->levelObjects.count(id)) 
		{
			LevelObject* child = level->levelObjects[id];
			child->setParent(obj);
		}
	}
}
