#pragma once

#include "Material.h"
#include "Shader.h"

struct DrawCommand
{
	int verticesCount;
	int indicesCount;
	int vertexOffset;
	int indexOffset;
	Material* material;
	Shader* shader;
};
