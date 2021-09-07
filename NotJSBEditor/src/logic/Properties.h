#pragma once

#include <optional>

#include "AnimationChannel.h"
#include "LevelObject.h"

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
	void insertChannelSelectable(LevelObject* levelObject, AnimationChannelType channelType, float defaultValue);
	std::string getChannelName(AnimationChannelType channelType) const;
};
