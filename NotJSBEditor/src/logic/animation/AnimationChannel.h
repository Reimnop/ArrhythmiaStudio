#pragma once

#include <vector>

#include "Keyframe.h"
#include "AnimationChannelType.h"

// Contains keyframes and also animates them
class AnimationChannel {
public:
	AnimationChannelType type;

	AnimationChannel(AnimationChannelType channelType, int count, Keyframe* keyframes);
	~AnimationChannel();

	void insertKeyframe(Keyframe keyframe);
	float update(float time);
private:
	std::vector<Keyframe> keyframes;

	float lastIndex;
};