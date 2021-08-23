#pragma once

#include "AnimationChannelType.h"
#include "animation/Sequence.h"

class AnimationChannel
{
public:
	AnimationChannelType type;
	Sequence* sequence;

	AnimationChannel(AnimationChannelType type, int count, Keyframe* keyframes);
	AnimationChannel(nlohmann::json j);
	~AnimationChannel();

	float update(float time) const;
	nlohmann::ordered_json toJson();
};
