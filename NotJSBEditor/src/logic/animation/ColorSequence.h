#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "ColorKeyframe.h"
#include "Color.h"

class ColorSequence
{
public:
	ColorSequence(int count, ColorKeyframe* keyframes);

	void insertKeyframe(ColorKeyframe keyframe);
	void eraseKeyframe(ColorKeyframe keyframe);

	Color update(float time);
private:
	float lastIndex;

	std::vector<ColorKeyframe> keyframes;
};
