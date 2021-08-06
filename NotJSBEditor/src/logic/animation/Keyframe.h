#pragma once

struct Keyframe
{
	float time;
	float value;

	bool operator==(Keyframe other)
	{
		return this->time == other.time && this->value == other.value;
	}
};
