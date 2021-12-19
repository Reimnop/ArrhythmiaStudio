#pragma once

#include "../animation/ColorSequence.h"

struct ColorKeyframeTimeEditInfo
{
	int index;
	ColorSequence* sequence;
	std::vector<ColorKeyframe> keyframes;
};
