#include "RemoveObjectCmd.h"
#include "../LevelManager.h"

RemoveObjectCmd::RemoveObjectCmd(LevelObject* obj)
{
	addCmd = std::make_unique<AddObjectCmd>(obj);
}

void RemoveObjectCmd::undo()
{
	addCmd->redo();
}

void RemoveObjectCmd::redo()
{
	addCmd->undo();
}
