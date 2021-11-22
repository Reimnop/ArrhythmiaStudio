#pragma once

#include <random>

class Utils
{
public:
	Utils() = delete;

	static uint64_t randomId()
	{
		return distId(rngId);
	}

	static float random()
	{
		return distF(rngF);
	}

	static std::string timeToString(float time)
	{
		float secs = std::floor(time);

		int seconds = (int)secs % 60;
		int minutes = (int)secs / 60 % 60;
		int hours = (int)secs / 3600 % 60;

		std::string secondsStr = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
		std::string minutesStr = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
		std::string hoursStr = hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);

		return hoursStr + ":" + minutesStr + ":" + secondsStr;
	}
private:
	static inline std::random_device rd;

	static inline std::mt19937_64 rngId = std::mt19937_64(rd());
	static inline std::uniform_int_distribution<uint64_t> distId;

	static inline std::mt19937_64 rngF = std::mt19937_64(rd());
	static inline std::uniform_real_distribution<float> distF;
};