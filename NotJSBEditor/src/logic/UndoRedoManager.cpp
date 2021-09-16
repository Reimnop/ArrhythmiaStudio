#include "UndoRedoManager.h"
#include "LevelManager.h"

#include <logger.h>

UndoRedoManager* UndoRedoManager::inst;

UndoRedoManager::UndoRedoManager()
{
	if (inst)
	{
		return;
	}

	inst = this;
}

void UndoRedoManager::push(UndoCommand* action)
{
	for (const UndoCommand* x : redoStack)
	{
		delete x;
	}

	redoStack.clear();
	undoStack.push_back(action);
}

void UndoRedoManager::reset()
{
	for (const UndoCommand* x : undoStack)
	{
		delete x;
	}

	for (const UndoCommand* x : redoStack)
	{
		delete x;
	}

	undoStack.clear();
	redoStack.clear();
}

void UndoRedoManager::undo()
{
	UndoCommand* cmd;
	if (popUndo(&cmd))
	{
		cmd->undo();
	}
}

void UndoRedoManager::redo()
{
	UndoCommand* cmd;
	if (popRedo(&cmd))
	{
		cmd->redo();
	}
}

bool UndoRedoManager::popUndo(UndoCommand** action)
{
	if (undoStack.empty())
	{
		return false;
	}

	UndoCommand* a = undoStack.back();

	*action = a;
	redoStack.push_back(a);
	undoStack.pop_back();

	return true;
}

bool UndoRedoManager::popRedo(UndoCommand** action)
{
	if (redoStack.empty())
	{
		return false;
	}

	UndoCommand* a = redoStack.back();

	*action = a;
	undoStack.push_back(a);
	redoStack.pop_back();

	return true;
}
