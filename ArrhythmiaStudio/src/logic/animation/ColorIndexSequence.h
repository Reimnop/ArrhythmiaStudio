#pragma once
#include <vector>

#include "../Level.h"
#include "Color.h"
#include "json.hpp"
#include "ColorIndexKeyframe.h"

using namespace nlohmann;

class ColorIndexSequence
{
public:
	std::vector<ColorIndexKeyframe> keyframes;

	ColorIndexSequence(Level* level);
	ColorIndexSequence(int count, ColorIndexKeyframe* keyframes, Level* level);
	ColorIndexSequence(json& j, Level* level);

	void loadKeyframes(std::vector<ColorIndexKeyframe>& keyframes);
	void insertKeyframe(ColorIndexKeyframe keyframe);
	void eraseKeyframe(ColorIndexKeyframe keyframe);

	Color update(float time);

	void fromJson(json& j);
	json toJson();
private:
	Level* level;
	int lastIndex;
};