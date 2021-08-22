#include "MeshRenderer.h"

#include "DrawElementsCommand.h"

MeshRenderer::MeshRenderer()
{
	mesh = nullptr;
	material = nullptr;

	// Create transform buffer
	glGenBuffers(1, uniformBuffers);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffers[0]);
	glBufferData(GL_UNIFORM_BUFFER, 256, nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

MeshRenderer::~MeshRenderer()
{
	glDeleteBuffers(1, uniformBuffers);
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

	uniformBuffers[1] = material->getUniformBuffer();

	// Update UBO
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffers[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 256, &input);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Decompose matrix
	glm::vec3 translation = input.model[3];

	// Construct the draw data for rendering
	DrawElementsCommand* drawCommand = new DrawElementsCommand();
	drawCommand->offset = 0;
	drawCommand->count = mesh->indicesCount;

	OutputDrawData* drawData = new OutputDrawData();
	drawData->vao = mesh->getVao();
	drawData->shader = material->getShader()->getHandle();
	drawData->uniformBuffersCount = 2;
	drawData->uniformBuffers = uniformBuffers;
	drawData->drawDepth = translation.z;
	drawData->drawTransparent = (opacity < 1.0f);
	drawData->renderCallback = [this]() { glUniform1f(0, opacity); };
	drawData->commandType = DrawCommandType_DrawElements;
	drawData->drawCommand = drawCommand;

	*output = drawData;

	return true;
}

bool MeshRenderer::canRender() const
{
	if (mesh == nullptr || material == nullptr)
		return false;

	return true;
}
