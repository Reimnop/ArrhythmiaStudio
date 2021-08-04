#pragma once

#include "DrawCommand.h"
#include "DrawCommandType.h"

// Data necessary for drawing
struct OutputDrawData
{
	uint32_t vao;
	uint32_t shader;
	int uniformBuffersCount;
	uint32_t* uniformBuffers;
	DrawCommandType commandType;
	DrawCommand* drawCommand;
};
