#pragma once

#include "LevelEventType.h"
#include "animation/Sequence.h"

class LevelEvent
{
public:
	LevelEventType type;
	Sequence* sequence;

	LevelEvent(LevelEventType type, int count, Keyframe* keyframes);
	LevelEvent(nlohmann::json j);
	~LevelEvent();

	float update(float time) const;
	nlohmann::ordered_json toJson();
};
