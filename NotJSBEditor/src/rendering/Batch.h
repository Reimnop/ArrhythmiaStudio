#pragma once

#include "Material.h"

struct Batch
{
	int verticesCount;
	int indicesCount;
	int vtxOffset;
	int idxOffset;
	Material* material;
};
