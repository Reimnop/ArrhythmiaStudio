#pragma once

#include "../UndoCommand.h"
#include "EventKeyframeAddCmd.h"

class EventKeyframeRemoveCmd : public UndoCommand
{
public:
	EventKeyframeRemoveCmd(LevelEventType type, Keyframe kf);
	~EventKeyframeRemoveCmd() override = default;

	void undo() override;
	void redo() override;
private:
	std::unique_ptr<EventKeyframeAddCmd> addCmd;
};