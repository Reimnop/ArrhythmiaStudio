#pragma once

#include <functional>

#include "DrawCommand.h"
#include "DrawCommandType.h"

// Data necessary for drawing
struct OutputDrawData
{
	uint32_t vao;
	uint32_t shader;
	int uniformBuffersCount;
	uint32_t* uniformBuffers;
	bool drawTransparent;
	float drawDepth; // This value is used for transparency and is ignored if drawTransparent is false
	std::function<void()> renderCallback;
	DrawCommandType commandType;
	DrawCommand* drawCommand;
};
