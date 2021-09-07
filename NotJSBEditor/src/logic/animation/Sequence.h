#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "Keyframe.h"

// Contains keyframes and also animates them
class Sequence
{
public:
	std::vector<Keyframe> keyframes;

	Sequence(int count, Keyframe* keyframes);
	Sequence(nlohmann::json j);

	void insertKeyframe(Keyframe keyframe);
	float update(float time);
	nlohmann::json toJson();
private:
	float lastIndex;
};
