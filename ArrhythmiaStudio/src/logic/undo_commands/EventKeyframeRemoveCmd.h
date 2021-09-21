#pragma once

#include "../UndoCommand.h"
#include "EventKeyframeAddCmd.h"

class EventKeyframeRemoveCmd : public UndoCommand
{
public:
	EventKeyframeRemoveCmd(const LevelEventType type, const Keyframe kf);
	~EventKeyframeRemoveCmd() override = default;

	void undo() override;
	void redo() override;
private:
	std::unique_ptr<EventKeyframeAddCmd> addCmd;
};