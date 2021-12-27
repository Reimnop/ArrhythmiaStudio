#pragma once

#include "Easing.h"
#include "json.hpp"

using namespace nlohmann;

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

	Keyframe(json j)
	{
		time = j[0].get<float>();
		value = j[1].get<float>();
		easing = j[2].get<EaseType>();
	}

	Keyframe(float time, float value, EaseType easing)
	{
		this->time = time;
		this->value = value;
		this->easing = easing;
	}

	bool operator==(const Keyframe& b) const
	{
		return this->time == b.time && this->value == b.value && this->easing == b.easing;
	}

	json toJson()
	{
		return { time, value, easing };
	}
};