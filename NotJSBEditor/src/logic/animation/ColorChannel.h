#pragma once

#include <vector>

#include "ColorKeyframe.h"
#include "Color.h"

class ColorChannel
{
public:
	std::vector<ColorKeyframe> keyframes;

	ColorChannel(int count, ColorKeyframe* keyframes);
	~ColorChannel();

	void insertKeyframe(ColorKeyframe keyframe);
	Color update(float time);
private:
	float lastIndex;

	float lerp(float a, float b, float t);
};
