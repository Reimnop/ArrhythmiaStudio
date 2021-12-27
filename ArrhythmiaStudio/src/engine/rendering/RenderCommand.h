#pragma once
#include <optional>

#include "DrawData.h"

struct RenderCommand
{
	std::optional<float> drawDepth;
	DrawData* drawData;

	RenderCommand(DrawData* drawData)
	{
		drawDepth = std::optional<float>();
		this->drawData = drawData;
	}

	~RenderCommand()
	{
		delete drawData;
	}
};
