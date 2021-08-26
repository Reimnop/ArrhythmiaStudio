#pragma once

#include "PostProcessor.h"
#include "../ComputeShader.h"
#include "../Texture2D.h"

class Bloom : public PostProcessor
{
public:
	Bloom();

	float intensity = 0.0f;

	void processImage(uint32_t image, int width, int height) override;
private:
	ComputeShader* blurDownsampleShader;
	ComputeShader* blurUpsampleShader;

	int lastWidth;
	int lastHeight;

	std::vector<Texture2D*> downsamplingTextures;
	std::vector<Texture2D*> upsamplingTextures;

	void resizeTextures(int width, int height);
};