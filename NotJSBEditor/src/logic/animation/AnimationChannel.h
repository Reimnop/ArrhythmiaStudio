#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "Keyframe.h"
#include "AnimationChannelType.h"

// Contains keyframes and also animates them
class AnimationChannel
{
public:
	AnimationChannelType type;
	std::vector<Keyframe> keyframes;

	AnimationChannel(AnimationChannelType channelType, int count, Keyframe* keyframes);
	AnimationChannel(nlohmann::json j);
	~AnimationChannel();

	void insertKeyframe(Keyframe keyframe);
	float update(float time);
	nlohmann::ordered_json toJson();
private:
	float lastIndex;

	float lerp(float a, float b, float t);
};
