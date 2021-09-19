#pragma once

#include <nlohmann/json.hpp>
#include <utils.h>

#include "Color.h"
#include "EaseType.h"

struct ColorKeyframe
{
	float time;
	Color color;
	EaseType easing;

	ColorKeyframe()
	{
		time = 0.0f;
		color = Color(1.0f, 1.0f, 1.0f);
		easing = EaseType_Linear;
	}

	ColorKeyframe(nlohmann::json j)
	{
		time = j["time"].get<float>();
		color.r = j["color"][0].get<float>();
		color.g = j["color"][1].get<float>();
		color.b = j["color"][2].get<float>();

		if (j.contains("ease"))
		{
			easing = j["ease"].get<EaseType>();
		}
		else
		{
			easing = EaseType_Linear;
		}
	}

	bool operator==(ColorKeyframe other) const
	{
		return time == other.time && color == other.color && easing == other.easing;
	}

	nlohmann::ordered_json toJson() const
	{
		nlohmann::ordered_json j;
		j["time"] = time;
		j["color"][0] = color.r;
		j["color"][1] = color.g;
		j["color"][2] = color.b;
		j["ease"] = easing;

		return j;
	}
};
