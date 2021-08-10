#pragma once

#include "LevelManager.h"

class Theme
{
public:
	Theme();
private:
	float startTime;
	float endTime;

	int selectedSlot = -1;

	void onLayout();
	bool colorSlotButton(std::string label, Color color, bool selected);
};
