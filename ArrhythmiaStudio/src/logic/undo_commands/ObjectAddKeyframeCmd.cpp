#include "ObjectAddKeyframeCmd.h"

#include "../LevelManager.h"

ObjectAddKeyframeCmd::ObjectAddKeyframeCmd(const uint64_t objectId, const AnimationChannelType type, const Keyframe kf)
{
	id = objectId;
	this->type = type;
	data = kf.toJson();
}

void ObjectAddKeyframeCmd::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];

	AnimationChannel* channel = obj->getChannel(type);
	channel->eraseKeyframe(Keyframe(data));

	levelManager->updateObject(obj);
}

void ObjectAddKeyframeCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];

	AnimationChannel* channel = obj->getChannel(type);
	channel->insertKeyframe(Keyframe(data));

	levelManager->updateObject(obj);
}
