#pragma once

#include "Color.h"

struct ColorKeyframe
{
	float time;
	Color color;

	bool operator==(ColorKeyframe b)
	{
		return this->time == b.time && this->color == b.color;
	}
};
