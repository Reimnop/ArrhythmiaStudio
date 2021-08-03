#include "AnimationChannel.h"

AnimationChannel::AnimationChannel(AnimationChannelType channelType, int count, Keyframe* keyframes) {
	for (int i = 0; i < count; i++) {
		insertKeyframe(keyframes[i]);
	}

	type = channelType;
	lastIndex = 0;
}

AnimationChannel::~AnimationChannel() {
	keyframes.clear();
	keyframes.shrink_to_fit();
}

bool keyframeComp(Keyframe a, Keyframe b) {
	return a.time < b.time;
}

void AnimationChannel::insertKeyframe(Keyframe keyframe) {
	if (keyframes.size() == 0) {
		keyframes.push_back(keyframe);
		return;
	}

	if (keyframes.back().time <= keyframe.time) {
		keyframes.push_back(keyframe);
		return;
	}

	std::vector<Keyframe>::iterator it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe, keyframeComp);
	keyframes.insert(it, keyframe);
}

float lerp(float a, float b, float t) {
	return (a * (1.0f - t)) + (b * t);
}

float AnimationChannel::update(float time) {
	// Bounds checking
	if (keyframes.size() == 0) {
		return 0.0f;
	}

	if (keyframes.size() == 1) {
		return keyframes.front().value;
	}

	if (time < keyframes.front().time) {
		return keyframes.front().value;
	}

	if (time >= keyframes.back().time) {
		return keyframes.back().value;
	}

	// If time is not out of bounds, find left and right keyframes
	Keyframe left, right;
	if (time >= keyframes[lastIndex].time && time < keyframes[lastIndex + 1].time) 
	{
		left = keyframes[lastIndex];
		right = keyframes[lastIndex + 1];
	}
	else 
	{
		if (time >= keyframes[lastIndex + 1].time) 
		{
			while (lastIndex + 1 < keyframes.size() - 1 && time >= keyframes[lastIndex + 1].time)
			{
				lastIndex++;
			}
		}
		else
		{
			while (lastIndex > 0 && time < keyframes[lastIndex].time) 
			{
				lastIndex--;
			}
		}

		left = keyframes[lastIndex];
		right = keyframes[lastIndex + 1];
	}

	// Apply easings
	float t = (time - left.time) / (right.time - left.time);
	return lerp(left.value, right.value, t);
}