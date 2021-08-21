#pragma once

#include <optional>

#include "animation/AnimationChannel.h"

class LevelManager;

class Properties
{
public:
	static Properties* inst;

	Properties();

	void reset();
private:
	float startTime;
	float endTime;

	std::optional<Keyframe> selectedKeyframe;
	AnimationChannel* selectedChannel;

	void onLayout();
	std::string getChannelName(AnimationChannelType channelType);
	float lerp(float a, float b, float t);
};
