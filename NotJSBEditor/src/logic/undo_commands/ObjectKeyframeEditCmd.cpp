#include "ObjectKeyframeEditCmd.h"
#include "../LevelManager.h"

#if 0
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
	const AnimationChannel* channel = *std::find_if(obj->animationChannels.begin(), obj->animationChannels.end(), 
		[this](const AnimationChannel* a)
		{
			return a->type == type;
		});

	Sequence* sequence = channel->sequence;

	const std::vector<Keyframe>::iterator it = std::remove(sequence->keyframes.begin(), sequence->keyframes.end(), newKf);
	sequence->keyframes.erase(it);
	sequence->insertKeyframe(oldKf);
	levelManager->updateObject(obj);
}

void ObjectKeyframeEditCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelObject* obj = levelManager->level->levelObjects[id];
	const AnimationChannel* channel = *std::find_if(obj->animationChannels.begin(), obj->animationChannels.end(),
		[this](const AnimationChannel* a)
		{
			return a->type == type;
		});

	Sequence* sequence = channel->sequence;

	const std::vector<Keyframe>::iterator it = std::remove(sequence->keyframes.begin(), sequence->keyframes.end(), oldKf);
	sequence->keyframes.erase(it);
	sequence->insertKeyframe(newKf);
	levelManager->updateObject(obj);
}
#endif