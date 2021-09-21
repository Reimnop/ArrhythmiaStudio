#pragma once

#include "LevelEventType.h"
#include "animation/Sequence.h"

class LevelEvent
{
public:
	std::vector<Keyframe> keyframes;

	LevelEventType type;
	Sequence* sequence;

	LevelEvent(LevelEventType type, int count, Keyframe* keyframes);
	LevelEvent(nlohmann::json j);
	~LevelEvent();

	void insertKeyframe(const Keyframe& keyframe);
	void eraseKeyframe(const Keyframe& keyframe);

	float update(float time) const;
	nlohmann::ordered_json toJson() const;
};
