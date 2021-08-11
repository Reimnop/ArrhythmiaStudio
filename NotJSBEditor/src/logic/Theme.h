#pragma once

#include <optional>
#include <string>

#include "animation/ColorKeyframe.h"

class Theme
{
public:
	Theme();
private:
	float startTime;
	float endTime;

	int selectedSlot = -1;
	std::optional<ColorKeyframe> selectedKeyframe;

	void onLayout();
	bool colorSlotButton(std::string label, Color color, bool selected);
};
