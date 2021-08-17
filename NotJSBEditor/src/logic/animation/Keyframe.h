#pragma once

#include <nlohmann/json.hpp>

struct Keyframe
{
	float time;
	float value;

	Keyframe()
	{
		time = 0.0f;
		value = 0.0f;
	}

	Keyframe(nlohmann::json j)
	{
		time = j[0].get<float>();
		value = j[1].get<float>();
	}

	Keyframe(float time, float value)
	{
		this->time = time;
		this->value = value;
	}

	bool operator==(Keyframe other) const
	{
		return this->time == other.time && this->value == other.value;
	}

	nlohmann::json toJson()
	{
		nlohmann::json j;
		j[0] = time;
		j[1] = value;

		return j;
	}
};
