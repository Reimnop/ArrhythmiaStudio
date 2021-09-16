#pragma once

struct UndoCommand
{
	virtual ~UndoCommand() = default;
	virtual void undo() = 0;
	virtual void redo() = 0;
};
