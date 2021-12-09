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
		int milliseconds = (time - floor(time)) * 1000.0f;
		int seconds = (int)time % 60;
		int minutes = (int)time / 60 % 60;
		int hours = (int)time / 3600 % 60;

		std::string msStr = milliseconds < 10 ? "00" + std::to_string(milliseconds) : milliseconds < 100 ? "0" + std::to_string(milliseconds) : std::to_string(milliseconds);
		std::string secondsStr = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
		std::string minutesStr = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
		std::string hoursStr = hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);

		return hoursStr + ":" + minutesStr + ":" + secondsStr + "." + msStr;
	}
private:
	static inline std::random_device rd;

	static inline std::mt19937_64 rngId = std::mt19937_64(rd());
	static inline std::uniform_int_distribution<uint64_t> distId;

	static inline std::mt19937_64 rngF = std::mt19937_64(rd());
	static inline std::uniform_real_distribution<float> distF;
};