#pragma once

#include <tuple>
#include <vector>
#include <functional>

#include "UndoCommand.h"

typedef std::function<void()> GenericCallback;
typedef std::tuple<UndoCommand*, GenericCallback, GenericCallback> CmdCallbacks;

class UndoRedoManager
{
public:
	static UndoRedoManager* inst;

	UndoRedoManager();

	void push(UndoCommand* action, GenericCallback undoCallback = nullptr, GenericCallback redoCallback = nullptr);
	void reset();
	void undo();
	void redo();
private:
	std::vector<CmdCallbacks> undoStack;
	std::vector<CmdCallbacks> redoStack;

	bool popUndo(CmdCallbacks* action);
	bool popRedo(CmdCallbacks* action);
};