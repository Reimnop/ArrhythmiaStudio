#include "EditObjectCmd.h"
#include "../LevelManager.h"

EditObjectCmd::EditObjectCmd(const uint64_t objectId, const LevelObjectProperties& oldState, const LevelObjectProperties& newState)
{
	id = objectId;
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
