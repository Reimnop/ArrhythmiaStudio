#pragma once

#include <vector>

#include "ColorKeyframe.h"
#include "Color.h"

class ColorSequence
{
public:
	std::vector<ColorKeyframe> keyframes;

	ColorSequence();
	ColorSequence(int count, ColorKeyframe* keyframes);
	ColorSequence(json& j);

	void loadKeyframes(std::vector<ColorKeyframe>& keyframes);
	void insertKeyframe(ColorKeyframe keyframe);
	void eraseKeyframe(ColorKeyframe keyframe);

	Color update(float time);

	void fromJson(json& j);
	json toJson();
private:
	float lastIndex;
};
