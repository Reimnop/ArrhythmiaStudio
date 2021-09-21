#pragma once

#include "../UndoCommand.h"
#include "../LevelEvent.h"

class AddLevelEventCmd : public UndoCommand
{
public:
	AddLevelEventCmd(const LevelEvent* levelEvent);
	~AddLevelEventCmd() override = default;

	void undo() override;
	void redo() override;
private:
	LevelEventType type;
	nlohmann::json data;
};