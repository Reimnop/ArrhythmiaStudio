#pragma once

#include "LevelEvent.h"
#include "animation/Keyframe.h"

class Events
{
public:
	Events();

	void reset();
private:
	float startTime;
	float endTime;

	LevelEvent* selectedEvent;
	int selectedKeyframeIndex = -1;

	Keyframe kfUndoState;

	void onLayout();
	std::string getEventName(LevelEventType type) const;
	void insertEventSelectable(LevelEventType type, float defaultValue);
};
