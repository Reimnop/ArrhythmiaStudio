#include "ObjectAddChannelCmd.h"

#include "../LevelManager.h"

ObjectAddChannelCmd::ObjectAddChannelCmd(const LevelObject* obj, const AnimationChannel* channel)
{
	id = obj->id;
	type = channel->type;
	data = channel->toJson();
}

void ObjectAddChannelCmd::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];
	obj->eraseChannel(type);

	levelManager->updateObject(obj);
}

void ObjectAddChannelCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];
	obj->insertChannel(new AnimationChannel(data));

	levelManager->updateObject(obj);
}
