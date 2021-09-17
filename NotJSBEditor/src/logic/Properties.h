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

	LevelObjectProperties oldObjectState;

	std::optional<Keyframe> selectedKeyframe;
	Keyframe keyframeOldState;
	AnimationChannel* selectedChannel;

	void onLayout();
	void insertChannelSelectable(LevelObject* levelObject, AnimationChannelType channelType, float defaultValue);
	std::string getChannelName(AnimationChannelType channelType) const;
};
