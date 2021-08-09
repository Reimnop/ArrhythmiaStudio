#pragma once

#include <functional>
#include <algorithm>
#include <optional>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include "../rendering/ImGuiController.h"
#include "animation/AnimationChannel.h"
#include "LevelManager.h"

class Theme
{
public:
	Theme();
private:
	int selectedSlot = -1;

	void onLayout();
	bool colorSlotButton(std::string label, Color color, bool selected);
};