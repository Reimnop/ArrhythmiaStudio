#pragma once

#include <vector>

#include "Keyframe.h"
#include "AnimationChannelType.h"

// Contains keyframes and also animates them
class AnimationChannel {
public:
	AnimationChannelType type;
	std::vector<Keyframe> keyframes;

	AnimationChannel(AnimationChannelType channelType, int count, Keyframe* keyframes);
	~AnimationChannel();

	void insertKeyframe(Keyframe keyframe);
	float update(float time);
private:
	float lastIndex;
};