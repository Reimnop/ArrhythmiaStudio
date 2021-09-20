#pragma once

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

	int selectedKeyframeIndex = -1;
	AnimationChannel* selectedChannel;

	Keyframe kfUndoState;

	void onLayout();
	void insertChannelSelectable(LevelObject* levelObject, AnimationChannelType channelType, float defaultValue);
	std::string getChannelName(AnimationChannelType channelType) const;
};
