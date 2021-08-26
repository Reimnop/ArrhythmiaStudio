#include "Bloom.h"

#include "glad/glad.h"
#include "../GlobalConstants.h"

Bloom::Bloom()
{
	downsampleShader = new ComputeShader("Assets/Shaders/PostProcessing/bloom-downsample.comp");
	upsampleShader = new ComputeShader("Assets/Shaders/PostProcessing/bloom-upsample.comp");

	lastWidth = RENDERER_INITIAL_WIDTH;
	lastHeight = RENDERER_INITIAL_HEIGHT;

	int w = RENDERER_INITIAL_WIDTH;
	int h = RENDERER_INITIAL_HEIGHT;

	for (int i = 0; i < 6; i++)
	{
		w /= 2;
		h /= 2;

		downsamplingTextures.push_back(new Texture2D(w, h, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT));
	}

	for (int i = 0; i < 5; i++)
	{
		w *= 2;
		h *= 2;

		upsamplingTextures.push_back(new Texture2D(w, h, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT));
	}
}

void Bloom::processImage(uint32_t image, int width, int height)
{
	if (intensity == 0.0f)
	{
		return;
	}

	// Resize if viewport size changes
	if (width != lastWidth || height != lastHeight)
	{
		resizeTextures(width, height);

		lastWidth = width;
		lastHeight = height;
	}

	// Downsampling
	for (int i = 0; i < downsamplingTextures.size(); i++)
	{
		int w, h;
		downsamplingTextures[i]->getSize(&w, &h);

		glUseProgram(downsampleShader->getHandle());

		glBindImageTexture(0, downsamplingTextures[i]->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, i == 0 ? image : downsamplingTextures[i - 1]->getHandle());

		glDispatchCompute(std::ceil(w / 8.0f), std::ceil(h / 8.0f), 1);
	}

	// Upsampling
	glUseProgram(upsampleShader->getHandle());

	glUniform1i(0, 0);
	glUniform1i(1, 1);
	glUniform1f(2, intensity);

	for (int i = 0; i < upsamplingTextures.size(); i++)
	{
		int w, h;
		upsamplingTextures[i]->getSize(&w, &h);

		glBindImageTexture(0, upsamplingTextures[i]->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, i == 0 ? downsamplingTextures.back()->getHandle() : upsamplingTextures[i - 1]->getHandle());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, downsamplingTextures[downsamplingTextures.size() - i - 2]->getHandle());

		glDispatchCompute(std::ceil(w / 8.0f), std::ceil(h / 8.0f), 1);
	}

	glBindImageTexture(0, image, 0, false, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, upsamplingTextures.back()->getHandle());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, image);

	glDispatchCompute(std::ceil(width / 8.0f), std::ceil(height / 8.0f), 1);
}

void Bloom::resizeTextures(int width, int height)
{
	int w = width;
	int h = height;

	for (int i = 0; i < downsamplingTextures.size(); i++)
	{
		w /= 2;
		h /= 2;

		downsamplingTextures[i]->resize(w, h);
	}

	for (int i = 0; i < upsamplingTextures.size(); i++)
	{
		w *= 2;
		h *= 2;

		upsamplingTextures[i]->resize(w, h);
	}
}
