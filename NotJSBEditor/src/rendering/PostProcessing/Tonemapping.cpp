#include "Tonemapping.h"

#include "glad/glad.h"

Tonemapping::Tonemapping()
{
	tonemappingShader = new ComputeShader("Assets/Shaders/PostProcessing/tonemapping.comp");
}

void Tonemapping::processImage(uint32_t image, int width, int height)
{
	glUseProgram(tonemappingShader->getHandle());
	glBindImageTexture(0, image, 0, false, 0, GL_READ_WRITE, GL_RGBA16F);
	glDispatchCompute(std::ceil(width / 8.0f), std::ceil(height / 8.0f), 1);
}
