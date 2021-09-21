#include "EventKeyframeAddCmd.h"

#include "../LevelManager.h"

EventKeyframeAddCmd::EventKeyframeAddCmd(LevelEventType type, Keyframe kf)
{
	this->type = type;
	data = kf.toJson();
}

void EventKeyframeAddCmd::undo()
{
	const LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;

	LevelEvent* levelEvent = level->getLevelEvent(type);
	levelEvent->eraseKeyframe(Keyframe(data));
}

void EventKeyframeAddCmd::redo()
{
	const LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;

	LevelEvent* levelEvent = level->getLevelEvent(type);
	levelEvent->insertKeyframe(Keyframe(data));
}
