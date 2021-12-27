#pragma once

#include <stdexcept>

#include "DrawData.h"
#include "Mesh.h"

// Batched drawing data format
struct BatchedDrawData : DrawData
{
	Mesh* mesh;
	glm::mat4 transform;
	glm::vec4 color;

	~BatchedDrawData() override = default;

	DrawDataType getType() const override
	{
		return DrawDataType_Batched;
	}

	Drawer* getDrawer() override
	{
		throw std::runtime_error("How do you even get here?");
	}
};
