#include "EventKeyframeRemoveCmd.h"

EventKeyframeRemoveCmd::EventKeyframeRemoveCmd(const LevelEventType type, const Keyframe kf)
{
	addCmd = std::make_unique<EventKeyframeAddCmd>(type, kf);
}

void EventKeyframeRemoveCmd::undo()
{
	addCmd->redo();
}

void EventKeyframeRemoveCmd::redo()
{
	addCmd->undo();
}
