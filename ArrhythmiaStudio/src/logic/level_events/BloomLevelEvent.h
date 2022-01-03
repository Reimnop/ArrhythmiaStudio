#pragma once

#include "AnimateableLevelEvent.h"

class BloomLevelEvent : public AnimateableLevelEvent
{
public:
	BloomLevelEvent(Level* level);
	~BloomLevelEvent() override = default;

	static LevelEvent* create(Level* level);

	std::string getTitle() override;

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;
protected:
	void drawSequences() override;
private:
	Sequence intensity;
	Sequence scatter;
	Sequence threshold;
};