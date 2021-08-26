#pragma once

#include "PostProcessor.h"
#include "../ComputeShader.h"

class Tonemapping : public PostProcessor
{
public:
	Tonemapping();

	void processImage(uint32_t image, int width, int height) override;
private:
	ComputeShader* tonemappingShader;
};