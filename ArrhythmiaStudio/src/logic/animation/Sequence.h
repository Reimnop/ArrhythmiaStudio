#pragma once
#include <vector>

#include "json.hpp"
#include "Keyframe.h"

using namespace nlohmann;

class Sequence
{
public:
	std::vector<Keyframe> keyframes;

	Sequence();
	Sequence(int count, Keyframe* keyframes);

	void loadKeyframes(std::vector<Keyframe>& keyframes);
	void insertKeyframe(Keyframe keyframe);
	void eraseKeyframe(Keyframe keyframe);

	float update(float time);

	void fromJson(json j);
	json toJson();
private:
	int lastIndex;
};
