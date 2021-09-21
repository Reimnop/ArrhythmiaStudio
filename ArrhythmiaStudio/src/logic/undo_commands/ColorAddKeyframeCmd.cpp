#include "ColorAddKeyframeCmd.h"
#include "../LevelManager.h"

ColorAddKeyframeCmd::ColorAddKeyframeCmd(int index, ColorKeyframe kf)
{
	this->index = index;
	data = kf.toJson();
}

void ColorAddKeyframeCmd::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;
	ColorSlot* slot = level->colorSlots[index];
	slot->eraseKeyframe(ColorKeyframe(data));
}

void ColorAddKeyframeCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	Level* level = levelManager->level;
	ColorSlot* slot = level->colorSlots[index];
	slot->insertKeyframe(ColorKeyframe(data));
}
