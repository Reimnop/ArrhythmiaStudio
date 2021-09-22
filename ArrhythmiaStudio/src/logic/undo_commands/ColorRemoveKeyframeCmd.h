#pragma once

#include "ColorAddKeyframeCmd.h"

class ColorRemoveKeyframeCmd : public UndoCommand
{
public:
	ColorRemoveKeyframeCmd(int index, ColorKeyframe kf);
	~ColorRemoveKeyframeCmd() override = default;

	void undo() override;
	void redo() override;
private:
	std::unique_ptr<ColorAddKeyframeCmd> addCmd;
};