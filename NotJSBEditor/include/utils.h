#pragma once

#include <random>

class Utils
{
public:
	Utils() = delete;

	static uint64_t randomId()
	{
		return dist(rng);
	}
private:
	static inline std::random_device rd;
	static inline std::mt19937_64 rng = std::mt19937_64(rd());
	static inline std::uniform_int_distribution<uint64_t> dist;
};