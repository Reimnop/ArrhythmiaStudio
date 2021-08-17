#pragma once

#include <nlohmann/json.hpp>

#include "Color.h"

struct ColorKeyframe
{
	float time;
	Color color;

	ColorKeyframe()
	{
		time = 0.0f;
		color = Color(1.0f, 1.0f, 1.0f);
	}

	ColorKeyframe(nlohmann::json j)
	{
		time = j["time"].get<float>();
		color.r = j["color"][0].get<float>();
		color.g = j["color"][1].get<float>();
		color.b = j["color"][2].get<float>();
	}

	bool operator==(ColorKeyframe b)
	{
		return this->time == b.time && this->color == b.color;
	}

	nlohmann::ordered_json toJson()
	{
		nlohmann::ordered_json j;
		j["time"] = time;
		j["color"][0] = color.r;
		j["color"][1] = color.g;
		j["color"][2] = color.b;

		return j;
	}
};
