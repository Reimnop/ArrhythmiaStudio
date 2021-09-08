#include "MeshRenderer.h"
MeshRenderer::MeshRenderer()
{
	mesh = nullptr;
	material = nullptr;
}

MeshRenderer::~MeshRenderer()
{
}

bool MeshRenderer::render(glm::mat4 transform, OutputDrawData** output)
{
	if (!canRender())
	{
		return false;
	}

	if (opacity == 0.0f)
	{
		return false;
	}

	// Construct the draw data for rendering
	OutputDrawData* drawData = new OutputDrawData();
	drawData->mesh = mesh;
	drawData->material = material;
	drawData->transform = transform;
	drawData->drawDepth = transform[3].z;
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
