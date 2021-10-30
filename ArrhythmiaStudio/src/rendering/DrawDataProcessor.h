#pragma once

#include "DrawData.h"
#include "BatchedDrawData.h"
#include "TextDrawData.h"

class DrawDataProcessor
{
public:
	DrawDataProcessor(glm::mat4 viewProjection);

	void processDrawData(const DrawData* drawData);
private:
	glm::mat4 viewProjection;

	uint32_t lastVertexArray = -1;
	uint32_t lastShader = -1;

	void processBatchedDrawData(const BatchedDrawData* drawData);
	void processTextDrawData(const TextDrawData* drawData);
};
