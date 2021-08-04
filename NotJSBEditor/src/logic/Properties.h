#pragma once

#include <functional>
#include <algorithm>

#include "../rendering/ImGuiController.h"
#include "animation/AnimationChannel.h"

class LevelManager;

class Properties
{
public:
	Properties(LevelManager* levelManager);
private:
	LevelManager* levelManager;

	float startTime;
	float endTime;

	void onLayout();
	const char* getChannelName(AnimationChannelType channelType);
};
