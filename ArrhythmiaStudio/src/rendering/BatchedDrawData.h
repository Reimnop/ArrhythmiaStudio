#pragma once

#include "DrawData.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

// Batched drawing data format
struct BatchedDrawData : DrawData
{
	Mesh* mesh;
	Material* material;
	Shader* shader;
	glm::mat4 transform;
	float opacity;

	DrawDataType getType() const override
	{
		return DrawDataType_Batched;
	}
};
