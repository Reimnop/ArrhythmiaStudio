#include "DrawDataProcessor.h"

DrawDataProcessor::DrawDataProcessor(glm::mat4 viewProjection)
{
	this->viewProjection = viewProjection;
}


void DrawDataProcessor::processDrawData(const DrawData* drawData)
{
	switch (drawData->getType())
	{
		case DrawDataType_Batched:
			processBatchedDrawData((BatchedDrawData*)drawData);
		case DrawDataType_Text:
			processTextDrawData((TextDrawData*)drawData);
	}
}

void DrawDataProcessor::processBatchedDrawData(const BatchedDrawData* drawData)
{
	if (!drawData->mesh || !drawData->material || !drawData->shader)
	{
		return;
	}

	const Material* material = drawData->material;
	const Shader* shader = drawData->shader;
	const Mesh* mesh = drawData->mesh;

	if (lastShader != shader->getHandle())
	{
		glUseProgram(shader->getHandle());
		lastShader = shader->getHandle();
	}

	if (lastVertexArray != mesh->getVao())
	{
		glBindVertexArray(mesh->getVao());
		lastVertexArray = mesh->getVao();
	}

	glm::mat4 mvp = viewProjection * drawData->transform;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, material->getUniformBuffer());

	glUniformMatrix4fv(0, 1, false, &mvp[0][0]);
	glUniform1f(1, drawData->opacity);

	glDrawElements(GL_TRIANGLES, mesh->indicesCount, GL_UNSIGNED_INT, nullptr);
}

void DrawDataProcessor::processTextDrawData(const TextDrawData* drawData)
{
	if (!drawData->vao || !drawData->material || !drawData->shader)
	{
		return;
	}

	const uint32_t vao = drawData->vao;
	const uint32_t shader = drawData->shader;
	const Material* material = drawData->material;

	if (lastShader != shader)
	{
		glUseProgram(shader);
		lastShader = shader;
	}

	if (lastVertexArray != vao)
	{
		glBindVertexArray(vao);
		lastVertexArray = vao;
	}

	glm::mat4 mvp = viewProjection * drawData->transform;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, material->getUniformBuffer());

	glUniformMatrix4fv(0, 1, false, &mvp[0][0]);
	glUniform1f(1, drawData->opacity);

	glBindTexture(GL_TEXTURE_2D, drawData->atlasTexture);

	glDrawArrays(GL_TRIANGLES, 0, drawData->count);
}
