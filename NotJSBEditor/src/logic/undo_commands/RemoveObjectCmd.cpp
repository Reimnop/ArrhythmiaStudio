#include "RemoveObjectCmd.h"
#include "../LevelManager.h"

RemoveObjectCmd::RemoveObjectCmd(LevelObject* obj)
{
	addCmd = new AddObjectCmd(obj);
}

RemoveObjectCmd::~RemoveObjectCmd()
{
	delete addCmd;
}

void RemoveObjectCmd::undo()
{
	addCmd->redo();
}

void RemoveObjectCmd::redo()
{
	addCmd->undo();
}
