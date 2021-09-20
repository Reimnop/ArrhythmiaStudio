#include "EditObjectCmd.h"
#include "../LevelManager.h"

EditObjectCmd::EditObjectCmd(const LevelObject* obj, const LevelObjectProperties& oldState, const LevelObjectProperties& newState)
{
	id = obj->id;
	this->oldState = oldState;
	this->newState = newState;
}

void EditObjectCmd::undo()
{
	LevelObject* obj = LevelManager::inst->level->levelObjects[id];
	obj->applyProperties(oldState);
}

void EditObjectCmd::redo()
{
	LevelObject* obj = LevelManager::inst->level->levelObjects[id];
	obj->applyProperties(newState);
}
