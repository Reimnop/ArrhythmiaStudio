#include "AddLevelEventCmd.h"

#include "../LevelManager.h"

AddLevelEventCmd::AddLevelEventCmd(const LevelEvent* levelEvent)
{
	type = levelEvent->type;
	data = levelEvent->toJson();
}

void AddLevelEventCmd::undo()
{
	const LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;
	level->eraseLevelEvent(type);
}

void AddLevelEventCmd::redo()
{
	const LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;
	level->insertLevelEvent(new LevelEvent(data));
}
