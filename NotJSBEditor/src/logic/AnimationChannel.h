#pragma once

#include "AnimationChannelType.h"
#include "animation/Sequence.h"

class AnimationChannel
{
public:
	std::vector<Keyframe> keyframes;

	AnimationChannelType type;
	Sequence* sequence;

	AnimationChannel(AnimationChannelType type, int count, Keyframe* keyframes);
	AnimationChannel(nlohmann::json j);
	~AnimationChannel();

	void insertKeyframe(const Keyframe& kf);
	void eraseKeyframe(const Keyframe& kf);

	float update(float time) const;
	nlohmann::ordered_json toJson() const;
};
