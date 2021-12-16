#include "MeshRenderer.h"

MeshRenderer::MeshRenderer()
{
	mesh = nullptr;
}

bool MeshRenderer::tryRender(glm::mat4 transform, RenderCommand** command)
{
	if (!canRender())
	{
		return false;
	}

	BatchedDrawData* drawData = new BatchedDrawData();
	drawData->mesh = mesh;
	drawData->transform = transform;
	drawData->opacity = opacity;

	RenderCommand* cmd = new RenderCommand(drawData);

	if (opacity < 1.0f)
	{
		cmd->drawDepth = transform[3].z;
	}

	*command = cmd;

	return true;
}

bool MeshRenderer::canRender() const
{
	return mesh != nullptr && opacity > 0.0f;
}
