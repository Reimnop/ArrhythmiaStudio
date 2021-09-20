#pragma once

#include "Mesh.h"
#include "Material.h"

// Data necessary for drawing
struct OutputDrawData
{
	Mesh* mesh;
	Material* material;
	glm::mat4 transform;
	bool drawTransparent;
	float drawDepth; // This value is used for transparency and is ignored if drawTransparent is false
	float opacity;
};
