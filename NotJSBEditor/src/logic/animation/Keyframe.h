#pragma once

#include <nlohmann/json.hpp>

#include "EaseType.h"

struct Keyframe
{
	float time;
	float value;
	EaseType easing;

	Keyframe()
	{
		time = 0.0f;
		value = 0.0f;
		easing = EaseType_Linear;
	}

	Keyframe(nlohmann::json j)
	{
		time = j[0].get<float>();
		value = j[1].get<float>();

		if (j.size() > 2)
		{
			easing = j[2].get<EaseType>();
		}
		else
		{
			easing = EaseType_Linear;
		}
	}

	Keyframe(float time, float value, EaseType easing = EaseType_Linear)
	{
		this->time = time;
		this->value = value;
		this->easing = easing;
	}

	bool operator==(Keyframe other) const
	{
		return time == other.time && value == other.value && easing == other.easing;
	}

	nlohmann::json toJson()
	{
		nlohmann::json j;
		j[0] = time;
		j[1] = value;
		j[2] = easing;

		return j;
	}
};
