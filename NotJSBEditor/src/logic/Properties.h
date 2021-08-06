#pragma once

#include <functional>
#include <algorithm>
#include <optional>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include "../rendering/ImGuiController.h"
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
