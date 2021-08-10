#pragma once

struct Keyframe
{
	float time;
	float value;

	bool operator==(Keyframe other) const
	{
		return this->time == other.time && this->value == other.value;
	}
};
