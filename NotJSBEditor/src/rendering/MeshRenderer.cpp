#include "MeshRenderer.h"
MeshRenderer::MeshRenderer()
{
	mesh = nullptr;
	material = nullptr;
}

MeshRenderer::~MeshRenderer()
{
}

bool MeshRenderer::render(InputDrawData input, OutputDrawData** output)
{
	if (!canRender())
	{
		return false;
	}

	if (opacity == 0.0f)
	{
		return false;
	}

	// Decompose matrix
	glm::vec3 translation = input.model[3];

	// Construct the draw data for rendering
	OutputDrawData* drawData = new OutputDrawData();
	drawData->mesh = mesh;
	drawData->material = material;
	drawData->transform = input.model;
	drawData->drawDepth = translation.z;
	drawData->drawTransparent = (opacity < 1.0f);
	drawData->opacity = opacity;

	*output = drawData;

	return true;
}

bool MeshRenderer::canRender() const
{
	if (mesh == nullptr || material == nullptr)
		return false;

	return true;
}
