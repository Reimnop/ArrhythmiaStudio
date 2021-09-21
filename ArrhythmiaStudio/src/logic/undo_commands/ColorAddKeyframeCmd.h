#pragma once

#include "../UndoCommand.h"
#include "../animation/ColorKeyframe.h"
#include "../ColorSlot.h"

class ColorAddKeyframeCmd : public UndoCommand
{
public:
	ColorAddKeyframeCmd(int index, ColorKeyframe kf);
	~ColorAddKeyframeCmd() override = default;

	void undo() override;
	void redo() override;
private:
	int index;
	nlohmann::json data;
};