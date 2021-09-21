#pragma once

#include "../UndoCommand.h"
#include "../LevelEvent.h"

class EventKeyframeEditCmd : public UndoCommand
{
public:
	EventKeyframeEditCmd(LevelEventType type, Keyframe oldKf, Keyframe newKf);
	~EventKeyframeEditCmd() override = default;

	void undo() override;
	void redo() override;
private:
	LevelEventType type;
	Keyframe oldKf;
	Keyframe newKf;
};