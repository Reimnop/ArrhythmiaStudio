#pragma once

#include "../UndoCommand.h"
#include "../LevelEvent.h"

class EventKeyframeAddCmd : public UndoCommand
{
public:
	EventKeyframeAddCmd(const LevelEventType type, const Keyframe kf);
	~EventKeyframeAddCmd() override = default;

	void undo() override;
	void redo() override;
private:
	LevelEventType type;
	nlohmann::json data;
};