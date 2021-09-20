#include "UndoRedoManager.h"
#include "LevelManager.h"

UndoRedoManager* UndoRedoManager::inst;

UndoRedoManager::UndoRedoManager()
{
	if (inst)
	{
		return;
	}

	inst = this;
}

void UndoRedoManager::push(UndoCommand* action, GenericCallback undoCallback, GenericCallback redoCallback)
{
	for (const CmdCallbacks x : redoStack)
	{
		delete std::get<0>(x);
	}

	redoStack.clear();
	undoStack.push_back(std::make_tuple(action, undoCallback, redoCallback));
}

void UndoRedoManager::reset()
{
	for (const CmdCallbacks x : undoStack)
	{
		delete std::get<0>(x);
	}

	for (const CmdCallbacks x : redoStack)
	{
		delete std::get<0>(x);
	}

	undoStack.clear();
	redoStack.clear();
}

void UndoRedoManager::undo()
{
	CmdCallbacks cmd;
	if (popUndo(&cmd))
	{
		UndoCommand* undoCommand = std::get<0>(cmd);
		const GenericCallback undoCallback = std::get<1>(cmd);

		undoCommand->undo();

		if (undoCallback)
		{
			undoCallback();
		}
	}
}

void UndoRedoManager::redo()
{
	CmdCallbacks cmd;
	if (popRedo(&cmd))
	{
		UndoCommand* undoCommand = std::get<0>(cmd);
		const GenericCallback redoCallback = std::get<1>(cmd);

		undoCommand->redo();

		if (redoCallback)
		{
			redoCallback();
		}
	}
}

bool UndoRedoManager::popUndo(CmdCallbacks* action)
{
	if (undoStack.empty())
	{
		return false;
	}

	const CmdCallbacks a = undoStack.back();

	*action = a;
	redoStack.push_back(a);
	undoStack.pop_back();

	return true;
}

bool UndoRedoManager::popRedo(CmdCallbacks* action)
{
	if (redoStack.empty())
	{
		return false;
	}

	const CmdCallbacks a = redoStack.back();

	*action = a;
	undoStack.push_back(a);
	redoStack.pop_back();

	return true;
}
