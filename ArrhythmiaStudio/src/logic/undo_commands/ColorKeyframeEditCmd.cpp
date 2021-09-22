#include "ColorKeyframeEditCmd.h"

#include "../LevelManager.h"

ColorKeyframeEditCmd::ColorKeyframeEditCmd(int index, ColorKeyframe oldKf, ColorKeyframe newKf)
{
	this->index = index;
	this->oldKf = oldKf;
	this->newKf = newKf;
}

void ColorKeyframeEditCmd::undo()
{
	LevelManager* levelManager = LevelManager::inst;
	ColorSlot* slot = levelManager->level->colorSlots[index];
	slot->sequence->eraseKeyframe(newKf);
	slot->sequence->insertKeyframe(oldKf);

	std::replace(slot->keyframes.begin(), slot->keyframes.end(), newKf, oldKf);

	levelManager->updateColorSlot(slot);
}

void ColorKeyframeEditCmd::redo()
{
	LevelManager* levelManager = LevelManager::inst;
	ColorSlot* slot = levelManager->level->colorSlots[index];
	slot->sequence->eraseKeyframe(oldKf);
	slot->sequence->insertKeyframe(newKf);

	std::replace(slot->keyframes.begin(), slot->keyframes.end(), oldKf, newKf);

	levelManager->updateColorSlot(slot);
}
