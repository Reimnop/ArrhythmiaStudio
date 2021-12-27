#pragma once

#include <cmath>
#include <string>

typedef float(*EaseFunc)(float);

constexpr float PI = 3.14159265358979323846f;

enum EaseType
{
	EaseType_Instant,
	EaseType_Linear,
	EaseType_InSine,
	EaseType_OutSine,
	EaseType_InOutSine,
	EaseType_InQuad,
	EaseType_OutQuad,
	EaseType_InOutQuad,
	EaseType_InCubic,
	EaseType_OutCubic,
	EaseType_InOutCubic,
	EaseType_InQuart,
	EaseType_OutQuart,
	EaseType_InOutQuart,
	EaseType_InQuint,
	EaseType_OutQuint,
	EaseType_InOutQuint,
	EaseType_InExpo,
	EaseType_OutExpo,
	EaseType_InOutExpo,
	EaseType_InCirc,
	EaseType_OutCirc,
	EaseType_InOutCirc,
	EaseType_InBack,
	EaseType_OutBack,
	EaseType_InOutBack,
	EaseType_InElastic,
	EaseType_OutElastic,
	EaseType_InOutElastic,
	EaseType_InBounce,
	EaseType_OutBounce,
	EaseType_InOutBounce,
	EaseType_Count
};

class Easing
{
public:
	Easing() = delete;

	static EaseFunc getEaseFunction(EaseType type)
	{
		switch (type)
		{
		case EaseType_Instant:
			return instant;
		case EaseType_Linear:
			return linear;
		case EaseType_InSine:
			return inSine;
		case EaseType_OutSine:
			return outSine;
		case EaseType_InOutSine:
			return inOutSine;
		case EaseType_InQuad:
			return inQuad;
		case EaseType_OutQuad:
			return outQuad;
		case EaseType_InOutQuad:
			return inOutQuad;
		case EaseType_InCubic:
			return inCubic;
		case EaseType_OutCubic:
			return outCubic;
		case EaseType_InOutCubic:
			return inOutCubic;
		case EaseType_InQuart:
			return inQuart;
		case EaseType_OutQuart:
			return outQuart;
		case EaseType_InOutQuart:
			return inOutQuart;
		case EaseType_InQuint:
			return inQuint;
		case EaseType_OutQuint:
			return outQuint;
		case EaseType_InOutQuint:
			return inOutQuint;
		case EaseType_InExpo:
			return inExpo;
		case EaseType_OutExpo:
			return outExpo;
		case EaseType_InOutExpo:
			return inOutExpo;
		case EaseType_InCirc:
			return inCirc;
		case EaseType_OutCirc:
			return outCirc;
		case EaseType_InOutCirc:
			return inOutCirc;
		case EaseType_InBack:
			return inBack;
		case EaseType_OutBack:
			return outBack;
		case EaseType_InOutBack:
			return inOutBack;
		case EaseType_InElastic:
			return inElastic;
		case EaseType_OutElastic:
			return outElastic;
		case EaseType_InOutElastic:
			return inOutElastic;
		case EaseType_InBounce:
			return inBounce;
		case EaseType_OutBounce:
			return outBounce;
		case EaseType_InOutBounce:
			return inOutBounce;
		}

		return linear;
	}

	static std::string getEaseName(EaseType type)
	{
		switch (type)
		{
		case EaseType_Instant:
			return "Instant";
		case EaseType_Linear:
			return "Linear";
		case EaseType_InSine:
			return "In Sine";
		case EaseType_OutSine:
			return "Out Sine";
		case EaseType_InOutSine:
			return "In Out Sine";
		case EaseType_InQuad:
			return "In Quad";
		case EaseType_OutQuad:
			return "Out Quad";
		case EaseType_InOutQuad:
			return "In Out Quad";
		case EaseType_InCubic:
			return "In Cubic";
		case EaseType_OutCubic:
			return "Out Cubic";
		case EaseType_InOutCubic:
			return "In Out Cubic";
		case EaseType_InQuart:
			return "In Quart";
		case EaseType_OutQuart:
			return "Out Quart";
		case EaseType_InOutQuart:
			return "In Out Quart";
		case EaseType_InQuint:
			return "In Quint";
		case EaseType_OutQuint:
			return "Out Quint";
		case EaseType_InOutQuint:
			return "In Out Quint";
		case EaseType_InExpo:
			return "In Expo";
		case EaseType_OutExpo:
			return "Out Expo";
		case EaseType_InOutExpo:
			return "In Out Expo";
		case EaseType_InCirc:
			return "In Circ";
		case EaseType_OutCirc:
			return "Out Circ";
		case EaseType_InOutCirc:
			return "In Out Circ";
		case EaseType_InBack:
			return "In Back";
		case EaseType_OutBack:
			return "Out Back";
		case EaseType_InOutBack:
			return "In Out Back";
		case EaseType_InElastic:
			return "In Elastic";
		case EaseType_OutElastic:
			return "Out Elastic";
		case EaseType_InOutElastic:
			return "In Out Elastic";
		case EaseType_InBounce:
			return "In Bounce";
		case EaseType_OutBounce:
			return "Out Bounce";
		case EaseType_InOutBounce:
			return "In Out Bounce";
		}

		return "Unknown easing";
	}

	static float instant(float t)
	{
		return t == 1.0f ? 1.0f : 0.0f;
	}

	static float linear(float t)
	{
		return t;
	}

	static float inSine(float t)
	{
		return 1.0f - std::cos((t * PI) / 2.0f);
	}

	static float outSine(float t)
	{
		return std::sin((t * PI) / 2.0f);
	}

	static float inOutSine(float t)
	{
		return -(std::cos(PI * t) - 1.0f) / 2.0f;
	}

	static float inQuad(float t)
	{
		return t * t;
	}

	static float outQuad(float t)
	{
		return 1.0f - (1.0f - t) * (1.0f - t);
	}

	static float inOutQuad(float t)
	{
		return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
	}

	static float inCubic(float t)
	{
		return t * t * t;
	}

	static float outCubic(float t)
	{
		return 1.0f - std::pow(1.0f - t, 3.0f);
	}

	static float inOutCubic(float t)
	{
		return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
	}

	static float inQuart(float t)
	{
		return t * t * t * t;
	}

	static float outQuart(float t)
	{
		return 1.0f - std::pow(1.0f - t, 4.0f);
	}

	static float inOutQuart(float t)
	{
		return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
	}

	static float inQuint(float t)
	{
		return t * t * t * t * t;
	}

	static float outQuint(float t)
	{
		return 1.0f - pow(1.0f - t, 5.0f);
	}

	static float inOutQuint(float t)
	{
		return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
	}

	static float inExpo(float t)
	{
		return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
	}

	static float outExpo(float t)
	{
		return t == 1.0f ? 1.0f : 1.0f - pow(2.0f, -10.0f * t);
	}

	static float inOutExpo(float t)
	{
		return t == 0.0f
			? 0.0f
			: t == 1.0f
			? 1.0f
			: t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f
			: (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
	}

	static float inCirc(float t)
	{
		return 1.0f - std::sqrt(1.0f - std::pow(t, 2.0f));
	}

	static float outCirc(float t)
	{
		return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
	}

	static float inOutCirc(float t)
	{
		return t < 0.5f
			? (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f
			: (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	}

	static float inBack(float t)
	{
		constexpr float c1 = 1.70158f;
		constexpr float c3 = c1 + 1.0f;

		return c3 * t * t * t - c1 * t * t;
	}

	static float outBack(float t)
	{
		constexpr float c1 = 1.70158f;
		constexpr float c3 = c1 + 1.0f;

		return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
	}

	static float inOutBack(float t)
	{
		constexpr float c1 = 1.70158f;
		constexpr float c2 = c1 * 1.525f;

		return t < 0.5f
			? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
			: (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
	}

	static float inElastic(float t)
	{
		constexpr float c4 = (2.0f * PI) / 3.0f;

		return t == 0.0f
			? 0.0f
			: t == 1.0f
			? 1.0f
			: -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
	}

	static float outElastic(float t)
	{
		constexpr float c4 = (2.0f * PI) / 3.0f;

		return t == 0.0f
			? 0.0f
			: t == 1.0f
			? 1.0f
			: std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
	}

	static float inOutElastic(float t)
	{
		constexpr float c5 = (2.0f * PI) / 4.5f;

		return t == 0.0f
			? 0.0f
			: t == 1.0f
			? 1.0f
			: t < 0.5f
			? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f
			: (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
	}

	static float inBounce(float t)
	{
		return std::pow(2.0f, 6.0f * (t - 1.0f)) * std::abs(std::sin(t * PI * 3.5f));
	}

	static float outBounce(float t)
	{
		return 1.0f - std::pow(2.0f, -6.0f * t) * std::abs(std::cos(t * PI * 3.5f));
	}

	static float inOutBounce(float t)
	{
		return t < 0.5f
			? 8.0f * std::pow(2.0f, 8.0f * (t - 1.0f)) * std::abs(std::sin(t * PI * 7.0f))
			: 1.0f - 8.0f * std::pow(2.0f, -8.0f * t) * std::abs(std::sin(t * PI * 7.0f));
	}
};