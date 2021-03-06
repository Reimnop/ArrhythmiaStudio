#pragma once

#include "AnimateableLevelEvent.h"

class CameraLevelEvent : public AnimateableLevelEvent
{
public:
	CameraLevelEvent(Level* level);
	~CameraLevelEvent() override = default;

	static LevelEvent* create(Level* level);

	std::string getTitle() override;

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;
protected:
	void drawSequences() override;
private:
	Sequence positionX;
	Sequence positionY;
	Sequence rotation;
	Sequence scale;
};