#pragma once

#include <nlohmann/json.hpp>
#include <utils.h>

#include "EaseType.h"

struct Keyframe
{
	float time;
	bool random;
	float values[2];
	EaseType easing;

	float evaluatedValue;

	Keyframe()
	{
		time = 0.0f;
		random = false;
		values[0] = 0.0f;
		values[1] = 0.0f;
		easing = EaseType_Linear;

		evaluatedValue = 0.0f;
	}

	Keyframe(nlohmann::json j)
	{
		time = j["time"].get<float>();
		random = j["value"].is_array();
		if (random)
		{
			values[0] = j["value"][0].get<float>();
			values[1] = j["value"][1].get<float>();
		}
		else
		{
			values[0] = j["value"].get<float>();
			values[1] = values[0];
		}

		easing = j["ease"].get<EaseType>();
	}

	Keyframe(float time, float value, EaseType easing = EaseType_Linear)
	{
		random = false;

		this->time = time;
		this->values[0] = value;
		this->values[1] = value;
		this->easing = easing;
	}

	Keyframe(float time, float values[2], EaseType easing = EaseType_Linear)
	{
		random = true;

		this->time = time;
		this->values[0] = values[0];
		this->values[1] = values[1];
		this->easing = easing;
	}

	void evaluateValue()
	{
		if (random)
		{
			const float l = values[1] - values[0];
			evaluatedValue = values[0] + Utils::random() * l;
		}
		else
		{
			evaluatedValue = values[0];
		}
	}

	bool operator==(Keyframe other) const
	{
		return time == other.time && random == other.random && values[0] == other.values[0] && values[1] == other.values[1] && easing == other.easing;
	}

	nlohmann::ordered_json toJson()
	{
		nlohmann::ordered_json j;
		j["time"] = time;

		if (random)
		{
			j["value"][0] = values[0];
			j["value"][1] = values[1];
		}
		else
		{
			j["value"] = values[0];
		}

		j["ease"] = easing;

		return j;
	}
};
