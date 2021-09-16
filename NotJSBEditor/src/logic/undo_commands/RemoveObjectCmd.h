#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"
#include "AddObjectCmd.h"

class RemoveObjectCmd : public UndoCommand
{
public:
	RemoveObjectCmd(LevelObject* obj);
	~RemoveObjectCmd() override;

	void undo() override;
	void redo() override;
private:
	AddObjectCmd* addCmd;
};