#pragma once

#include "../Level.h"

class LevelEvent
{
public:
	LevelEvent(Level* level) : level(level) {}
	virtual ~LevelEvent() = default;

	virtual std::string getTitle() = 0;

	virtual void update(float time) = 0;

	virtual void drawEditor() = 0;
private:
	Level* level;
};