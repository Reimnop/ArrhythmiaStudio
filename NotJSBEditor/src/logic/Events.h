#pragma once

#include <optional>

#include "LevelEventType.h"
#include "animation/Keyframe.h"

class Events
{
public:
	Events();

	void reset();
private:
	float startTime;
	float endTime;

	int selectedEventIndex = -1;
	std::optional<Keyframe> selectedKeyframe;

	void onLayout();
	std::string getEventName(LevelEventType type) const;
	void insertEventSelectable(LevelEventType type, float defaultValue) const;
	float lerp(float a, float b, float t);
};
