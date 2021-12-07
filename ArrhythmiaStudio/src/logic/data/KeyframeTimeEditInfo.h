#pragma once

#include "../animation/Sequence.h"

struct KeyframeTimeEditInfo
{
	int index;
	Sequence* sequence;
	std::vector<Keyframe> keyframes;
};
