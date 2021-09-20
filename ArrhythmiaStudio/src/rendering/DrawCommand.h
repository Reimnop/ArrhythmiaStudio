#pragma once

#include "Material.h"

struct DrawCommand
{
	int verticesCount;
	int indicesCount;
	int vertexOffset;
	int indexOffset;
	Material* material;
};
