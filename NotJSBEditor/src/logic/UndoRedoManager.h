#pragma once

#include <vector>

#include "UndoCommand.h"

class UndoRedoManager
{
public:
	static UndoRedoManager* inst;

	UndoRedoManager();

	void push(UndoCommand* action);
	void reset();
	void undo();
	void redo();
private:
	std::vector<UndoCommand*> undoStack;
	std::vector<UndoCommand*> redoStack;

	bool popUndo(UndoCommand** action);
	bool popRedo(UndoCommand** action);
};