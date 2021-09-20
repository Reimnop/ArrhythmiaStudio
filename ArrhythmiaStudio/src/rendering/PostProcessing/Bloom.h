#pragma once

#include "PostProcessor.h"
#include "../ComputeShader.h"
#include "../Texture2D.h"

constexpr int MIP_CHAIN_SIZE = 6;

class Bloom : public PostProcessor
{
public:
	Bloom();

	float intensity = 0.0f;
	float scatter = 0.9f;

	void processImage(uint32_t image, int width, int height) override;
private:
	int mipWidths[MIP_CHAIN_SIZE];
	int mipHeights[MIP_CHAIN_SIZE];

	ComputeShader* blurHorizontal;
	ComputeShader* blurVertical;
	ComputeShader* upsampleShader;
	ComputeShader* combineShader;

	int lastWidth;
	int lastHeight;

	std::vector<Texture2D*> mipChain;
	Texture2D* bloomTex;
	
	void resizeMips(int width, int height);
	void genMips(int width, int height);

	void recalculateMipSizes(int width, int height);
};