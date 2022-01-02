#pragma once

#include "Easing.h"
#include "json.hpp"

using namespace nlohmann;

struct ColorIndexKeyframe
{
	float time;
	int value;
	EaseType easing;

	ColorIndexKeyframe()
	{
		time = 0.0f;
		value = 0.0f;
		easing = EaseType_Linear;
	}

	ColorIndexKeyframe(json j)
	{
		time = j[0].get<float>();
		value = j[1].get<int>();
		easing = j[2].get<EaseType>();
	}

	ColorIndexKeyframe(float time, int value, EaseType easing)
	{
		this->time = time;
		this->value = value;
		this->easing = easing;
	}

	bool operator==(const ColorIndexKeyframe& b) const
	{
		return this->time == b.time && this->value == b.value && this->easing == b.easing;
	}

	json toJson()
	{
		return { time, value, easing };
	}
};