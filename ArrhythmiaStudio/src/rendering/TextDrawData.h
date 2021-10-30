#pragma once

#include "DrawData.h"
#include "Material.h"

struct TextDrawData : DrawData
{
	uint32_t vao;
	uint32_t shader;
	Material* material;
	uint32_t atlasTexture;
	size_t count;
	glm::mat4 transform;
	float opacity;

	DrawDataType getType() const override
	{
		return DrawDataType_Text;
	}
};
