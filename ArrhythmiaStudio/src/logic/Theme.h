#pragma once

#include <string>

#include "animation/ColorKeyframe.h"

class Theme
{
public:
	Theme();

	void reset();
private:
	float startTime;
	float endTime;

	int selectedSlotIndex = -1;
	int selectedKeyframeIndex = -1;

	void onLayout();
	bool colorSlotButton(std::string label, Color color, bool selected) const;
};
