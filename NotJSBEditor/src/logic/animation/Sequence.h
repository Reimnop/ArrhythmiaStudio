#pragma once

#include <vector>

#include "Keyframe.h"

// Contains keyframes and also animates them
class Sequence
{
public:
	Sequence(int count, Keyframe* keyframes);
	
	void insertKeyframe(Keyframe keyframe);
	void eraseKeyframe(Keyframe keyframe);

	float update(float time);
private:
	std::vector<Keyframe> keyframes;

	float lastIndex;
};
