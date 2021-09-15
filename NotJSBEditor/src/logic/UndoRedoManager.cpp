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

void UndoRedoManager::push(const UndoAction& action)
{
	redoStack.clear();
	undoStack.push_back(action);
}

void UndoRedoManager::reset()
{
	undoStack.clear();
	redoStack.clear();
}

void UndoRedoManager::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;

	UndoAction action;
	if (popUndo(&action))
	{
		Logger::info("Undo: " + undoActionTypeToString(action.type));
		switch (action.type)
		{
		case UndoActionType_AddObject:
		{
			const uint64_t id = action.data["id"].get<uint64_t>();
			levelManager->removeObject(level->levelObjects[id]);
			levelManager->recalculateActionIndex(levelManager->time);
		}
		break;
		case UndoActionType_RemoveObject:
		{
			LevelObject* obj = new LevelObject(action.data);
			levelManager->insertObject(obj);
			levelManager->initializeObjectParent(obj);
			levelManager->recalculateActionIndex(levelManager->time);
		}
		break;
		case UndoActionType_StartTimeEdit:
		{
			const uint64_t id = action.data["id"].get<uint64_t>();
			LevelObject* obj = level->levelObjects[id];
			obj->startTime = action.data["old"].get<float>();
		}
		break;
		case UndoActionType_KillTimeEdit:
		{
			const uint64_t id = action.data["id"].get<uint64_t>();
			LevelObject* obj = level->levelObjects[id];
			obj->killTime = action.data["old"].get<float>();
		}
		break;
		}
	}
}

void UndoRedoManager::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;

	UndoAction action;
	if (popRedo(&action))
	{
		Logger::info("Redo: " + undoActionTypeToString(action.type));
		switch (action.type)
		{
		case UndoActionType_AddObject:
		{
			LevelObject* obj = new LevelObject(action.data);
			levelManager->insertObject(obj);
			levelManager->initializeObjectParent(obj);
			levelManager->recalculateActionIndex(levelManager->time);
		}
		break;
		case UndoActionType_RemoveObject:
		{
			const uint64_t id = action.data["id"].get<uint64_t>();
			levelManager->removeObject(level->levelObjects[id]);
			levelManager->recalculateActionIndex(levelManager->time);
		}
		break;
		case UndoActionType_StartTimeEdit:
		{
			const uint64_t id = action.data["id"].get<uint64_t>();
			LevelObject* obj = level->levelObjects[id];
			obj->startTime = action.data["new"].get<float>();
		}
		break;
		case UndoActionType_KillTimeEdit:
		{
			const uint64_t id = action.data["id"].get<uint64_t>();
			LevelObject* obj = level->levelObjects[id];
			obj->killTime = action.data["new"].get<float>();
		}
		break;
		}
	}
}

bool UndoRedoManager::popUndo(UndoAction* action)
{
	if (undoStack.empty())
	{
		return false;
	}

	const UndoAction a = undoStack.back();

	*action = a;
	redoStack.push_back(a);
	undoStack.pop_back();

	return true;
}

bool UndoRedoManager::popRedo(UndoAction* action)
{
	if (redoStack.empty())
	{
		return false;
	}

	const UndoAction a = redoStack.back();

	*action = a;
	undoStack.push_back(a);
	redoStack.pop_back();

	return true;
}

std::string UndoRedoManager::undoActionTypeToString(UndoActionType type) const
{
	switch (type)
	{
	case UndoActionType_AddObject:
		return "Add object";
	case UndoActionType_RemoveObject:
		return "Remove object";
	case UndoActionType_StartTimeEdit:
		return "Start time edit";
	case UndoActionType_KillTimeEdit:
		return "Kill time edit";
	}

	return "Unknown action";
}
