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
	const char* getChannelName(AnimationChannelType channelType);
};
