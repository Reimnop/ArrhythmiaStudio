#include "Bloom.h"

#include "glad/glad.h"
#include "../GlobalConstants.h"

Bloom::Bloom()
{
	blurHorizontal = new ComputeShader("Assets/Shaders/PostProcessing/bloom-blur-horizontal.comp");
	blurVertical = new ComputeShader("Assets/Shaders/PostProcessing/bloom-blur-vertical.comp");
	upsampleShader = new ComputeShader("Assets/Shaders/PostProcessing/bloom-upsample.comp");
	combineShader = new ComputeShader("Assets/Shaders/PostProcessing/bloom-combine.comp");

	lastWidth = RENDERER_INITIAL_WIDTH;
	lastHeight = RENDERER_INITIAL_HEIGHT;

	genMips(RENDERER_INITIAL_WIDTH, RENDERER_INITIAL_HEIGHT);
	bloomTex = new Texture2D(RENDERER_INITIAL_WIDTH, RENDERER_INITIAL_HEIGHT, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);
}

void Bloom::processImage(uint32_t image, int width, int height)
{
	if (intensity == 0.0f)
	{
		return;
	}

	if (lastWidth != width || lastHeight != height)
	{
		resizeMips(width, height);
		bloomTex->resize(width, height);

		lastWidth = width;
		lastHeight = height;
	}

	// Downsampling
	glActiveTexture(GL_TEXTURE0);

	for (int i = 0; i < MIP_CHAIN_SIZE; i++) 
	{
		// Horizontal gaussian blur
		glUseProgram(blurHorizontal->getHandle());

		glBindImageTexture(0, mipChain[i]->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);
		glBindTexture(GL_TEXTURE_2D, i == 0 ? image : mipChain[i - 1]->getHandle());

		glDispatchCompute(std::ceil(mipWidths[i] / 8.0f), std::ceil(mipHeights[i] / 8.0f), 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Vertical gaussian blur
		glUseProgram(blurVertical->getHandle());

		glBindImageTexture(0, mipChain[i]->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);
		glBindTexture(GL_TEXTURE_2D, mipChain[i]->getHandle());

		glDispatchCompute(std::ceil(mipWidths[i] / 8.0f), std::ceil(mipHeights[i] / 8.0f), 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	// Upsampling
	glUseProgram(upsampleShader->getHandle());

	glUniform1i(0, 0);
	glUniform1i(1, 1);
	glUniform1f(2, scatter);

	for (int i = MIP_CHAIN_SIZE; i < MIP_CHAIN_SIZE * 2 - 1; i++)
	{
		const int j = 2 * MIP_CHAIN_SIZE - i - 2;

		glBindImageTexture(0, mipChain[i]->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mipChain[i - 1]->getHandle());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mipChain[j]->getHandle());

		glDispatchCompute(std::ceil(mipWidths[j] / 8.0f), std::ceil(mipHeights[j] / 8.0f), 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	glBindImageTexture(0, bloomTex->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mipChain.back()->getHandle());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, image);

	glDispatchCompute(std::ceil(width / 8.0f), std::ceil(height / 8.0f), 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Combine source and bloom texture
	glUseProgram(combineShader->getHandle());

	glUniform1i(0, 0);
	glUniform1i(1, 1);
	glUniform1f(2, intensity);

	glBindImageTexture(0, image, 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomTex->getHandle());

	glDispatchCompute(std::ceil(width / 8.0f), std::ceil(height / 8.0f), 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Bloom::resizeMips(int width, int height)
{
	recalculateMipSizes(width, height);

	for (int i = 0; i < MIP_CHAIN_SIZE; i++)
	{
		mipChain[i]->resize(mipWidths[i], mipHeights[i]);
	}

	for (int i = MIP_CHAIN_SIZE; i < MIP_CHAIN_SIZE * 2 - 1; i++)
	{
		const int j = 2 * MIP_CHAIN_SIZE - i - 2;
		mipChain[i]->resize(mipWidths[j], mipHeights[j]);
	}
}

void Bloom::genMips(int width, int height)
{
	recalculateMipSizes(width, height);

	for (int i = 0; i < MIP_CHAIN_SIZE; i++)
	{
		mipChain.push_back(new Texture2D(mipWidths[i], mipHeights[i], GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT));
	}

	for (int i = MIP_CHAIN_SIZE; i < MIP_CHAIN_SIZE * 2 - 1; i++)
	{
		const int j = 2 * MIP_CHAIN_SIZE - i - 2;
		mipChain.push_back(new Texture2D(mipWidths[j], mipHeights[j], GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT));
	}
}

void Bloom::recalculateMipSizes(int width, int height)
{
	for (int i = 0; i < MIP_CHAIN_SIZE; i++)
	{
		width /= 2;
		height /= 2;
		mipWidths[i] = width;
		mipHeights[i] = height;
	}
}
