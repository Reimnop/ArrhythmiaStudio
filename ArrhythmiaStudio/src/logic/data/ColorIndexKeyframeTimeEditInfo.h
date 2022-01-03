#pragma once

#include "../animation/ColorIndexSequence.h"

struct ColorIndexKeyframeTimeEditInfo
{
	int index;
	ColorIndexSequence* sequence;
	std::vector<ColorIndexKeyframe> keyframes;
};
