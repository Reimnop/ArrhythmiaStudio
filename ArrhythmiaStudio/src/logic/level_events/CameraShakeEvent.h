#pragma once
#include "LevelEvent.h"

class CameraShakeEvent : LevelEvent
{
public:
	CameraShakeEvent(Level* level);
	~CameraShakeEvent() override;

	std::string getTitle() override;
	void update(float time) override;
	void drawEditor() override;
};
