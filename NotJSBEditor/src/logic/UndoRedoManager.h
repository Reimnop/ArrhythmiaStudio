#pragma once

#include <string>
#include <vector>

#include "UndoAction.h"

class UndoRedoManager
{
public:
	static UndoRedoManager* inst;

	UndoRedoManager();

	void push(const UndoAction& action);
	void reset();
	void undo();
	void redo();
private:
	std::vector<UndoAction> undoStack;
	std::vector<UndoAction> redoStack;

	bool popUndo(UndoAction* action);
	bool popRedo(UndoAction* action);

	std::string undoActionTypeToString(UndoActionType type) const;
};