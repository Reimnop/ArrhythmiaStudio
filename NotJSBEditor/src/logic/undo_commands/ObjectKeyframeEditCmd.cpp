#include "ObjectKeyframeEditCmd.h"
#include "../LevelManager.h"

ObjectKeyframeEditCmd::ObjectKeyframeEditCmd(const LevelObject* obj, const AnimationChannelType channelType, const Keyframe& oldKf, const Keyframe& newKf)
{
	id = obj->id;
	type = channelType;
	this->oldKf = oldKf;
	this->newKf = newKf;
}

void ObjectKeyframeEditCmd::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];
	AnimationChannel* channel = *std::find_if(obj->animationChannels.begin(), obj->animationChannels.end(), 
		[this](const AnimationChannel* a)
		{
			return a->type == type;
		});

	Sequence* sequence = channel->sequence;

	sequence->eraseKeyframe(newKf);
	sequence->insertKeyframe(oldKf);

	std::replace(channel->keyframes.begin(), channel->keyframes.end(), newKf, oldKf);

	levelManager->updateObject(obj);
}

void ObjectKeyframeEditCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];
	AnimationChannel* channel = *std::find_if(obj->animationChannels.begin(), obj->animationChannels.end(),
		[this](const AnimationChannel* a)
		{
			return a->type == type;
		});

	Sequence* sequence = channel->sequence;

	sequence->eraseKeyframe(oldKf);
	sequence->insertKeyframe(newKf);

	std::replace(channel->keyframes.begin(), channel->keyframes.end(), oldKf, newKf);

	levelManager->updateObject(obj);
}