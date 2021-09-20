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
private:
	static inline std::random_device rd;

	static inline std::mt19937_64 rngId = std::mt19937_64(rd());
	static inline std::uniform_int_distribution<uint64_t> distId;

	static inline std::mt19937_64 rngF = std::mt19937_64(rd());
	static inline std::uniform_real_distribution<float> distF;
};