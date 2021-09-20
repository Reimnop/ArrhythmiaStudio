#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"

class EditObjectCmd : public UndoCommand
{
public:
	EditObjectCmd(const LevelObject* obj, const LevelObjectProperties& oldState, const LevelObjectProperties& newState);
	~EditObjectCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	LevelObjectProperties oldState;
	LevelObjectProperties newState;
};