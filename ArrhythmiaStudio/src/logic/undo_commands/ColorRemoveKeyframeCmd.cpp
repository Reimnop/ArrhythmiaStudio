#include "ColorRemoveKeyframeCmd.h"

ColorRemoveKeyframeCmd::ColorRemoveKeyframeCmd(int index, ColorKeyframe kf)
{
	addCmd = std::make_unique<ColorAddKeyframeCmd>(index, kf);
}

void ColorRemoveKeyframeCmd::undo()
{
	addCmd->redo();
}

void ColorRemoveKeyframeCmd::redo()
{
	addCmd->undo();
}
