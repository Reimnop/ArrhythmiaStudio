#pragma once

#include <optional>
#include <algorithm>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "../rendering/ImGuiController.h"

class LevelManager;

class Timeline
{
public:
	static Timeline* inst;

	Timeline();
private:
	float startTime;
	float endTime;

	void onLayout();
};
