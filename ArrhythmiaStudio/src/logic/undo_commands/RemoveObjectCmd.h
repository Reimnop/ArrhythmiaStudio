#pragma once

#include <memory>

#include "../UndoCommand.h"
#include "../LevelObject.h"
#include "AddObjectCmd.h"

class RemoveObjectCmd : public UndoCommand
{
public:
	RemoveObjectCmd(LevelObject* obj);
	~RemoveObjectCmd() override = default;

	void undo() override;
	void redo() override;
private:
	std::unique_ptr<AddObjectCmd> addCmd;
};