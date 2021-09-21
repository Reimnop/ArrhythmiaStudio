#include "EventKeyframeEditCmd.h"
#include "../LevelManager.h"

EventKeyframeEditCmd::EventKeyframeEditCmd(LevelEventType type, Keyframe oldKf, Keyframe newKf)
{
	this->type = type;
	this->oldKf = oldKf;
	this->newKf = newKf;
}

void EventKeyframeEditCmd::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelEvent* levelEvent = levelManager->level->getLevelEvent(type);
	levelEvent->sequence->eraseKeyframe(newKf);
	levelEvent->sequence->insertKeyframe(oldKf);

	std::replace(levelEvent->keyframes.begin(), levelEvent->keyframes.end(), newKf, oldKf);

	levelManager->updateLevelEvent(levelEvent);
}

void EventKeyframeEditCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	LevelEvent* levelEvent = levelManager->level->getLevelEvent(type);
	levelEvent->sequence->eraseKeyframe(oldKf);
	levelEvent->sequence->insertKeyframe(newKf);

	std::replace(levelEvent->keyframes.begin(), levelEvent->keyframes.end(), oldKf, newKf);

	levelManager->updateLevelEvent(levelEvent);
}
