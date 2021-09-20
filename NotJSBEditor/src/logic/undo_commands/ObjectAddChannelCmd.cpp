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
	const LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];
	obj->eraseChannel(type);
}

void ObjectAddChannelCmd::redo()
{
	const LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];
	obj->insertChannel(new AnimationChannel(data));
}
