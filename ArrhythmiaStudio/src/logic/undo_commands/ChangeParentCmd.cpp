#include "ChangeParentCmd.h"
#include "../LevelManager.h"

ChangeParentCmd::ChangeParentCmd(uint64_t objectId, const LevelObject* oldParent, const LevelObject* newParent)
{
	id = objectId;

	oldParentId = 0;
	if (oldParent)
	{
		oldParentId = oldParent->id;
	}

	newParentId = 0;
	if (newParent)
	{
		newParentId = newParent->id;
	}
}

void ChangeParentCmd::undo()
{
	Level* level = LevelManager::inst->level;
	LevelObject* curObj = level->levelObjects[id];

	if (oldParentId)
	{
		curObj->setParent(level->levelObjects[oldParentId]);
	}
	else
	{
		curObj->setParent(nullptr);
	}
}

void ChangeParentCmd::redo()
{
	Level* level = LevelManager::inst->level;
	LevelObject* curObj = level->levelObjects[id];

	if (newParentId)
	{
		curObj->setParent(level->levelObjects[newParentId]);
	}
	else
	{
		curObj->setParent(nullptr);
	}
}
