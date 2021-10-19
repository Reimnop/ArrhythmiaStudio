#pragma once

#include <cstdint>

class PostProcessor
{
public:
	virtual void processImage(uint32_t image, int width, int height) = 0;
};