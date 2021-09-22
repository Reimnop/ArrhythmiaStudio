#pragma once

#include "../UndoCommand.h"
#include "../ColorSlot.h"

class ColorKeyframeEditCmd : public UndoCommand
{
public:
	ColorKeyframeEditCmd(int index, ColorKeyframe oldKf, ColorKeyframe newKf);
	~ColorKeyframeEditCmd() override = default;

	void undo() override;
	void redo() override;
private:
	int index;
	ColorKeyframe oldKf;
	ColorKeyframe newKf;
};