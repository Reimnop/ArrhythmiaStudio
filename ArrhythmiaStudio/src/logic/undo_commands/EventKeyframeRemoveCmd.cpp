#include "EventKeyframeRemoveCmd.h"

EventKeyframeRemoveCmd::EventKeyframeRemoveCmd(LevelEventType type, Keyframe kf)
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
