#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "ColorKeyframe.h"
#include "Color.h"

class ColorChannel
{
public:
	std::vector<ColorKeyframe> keyframes;

	ColorChannel(int count, ColorKeyframe* keyframes);
	ColorChannel(nlohmann::json j);

	void insertKeyframe(ColorKeyframe keyframe);
	Color update(float time);
	nlohmann::ordered_json toJson();
private:
	float lastIndex;

	float lerp(float a, float b, float t);
};
