#include "TextRenderer.h"
#include "TextDrawData.h"

TextRenderer::TextRenderer(Font* font)
{
	this->font = font;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(TextVertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(TextVertex), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

TextRenderer::~TextRenderer()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void TextRenderer::setText(const std::wstring& text)
{
	if (!text.length())
	{
		count = 0;
		return;
	}

	TextMeshGenerator generator(font);

	std::vector<TextVertex> vertices = generator.genMesh(text);
	count = vertices.size();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(TextVertex), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool TextRenderer::tryRender(glm::mat4 transform, RenderCommand** command)
{
	if (!canRender())
	{
		return false;
	}

	TextDrawData* drawData = new TextDrawData();
	drawData->vao = vao;
	drawData->font = font;
	drawData->count = count;
	drawData->transform = transform;
	drawData->color = color;

	RenderCommand* cmd = new RenderCommand(drawData);
	cmd->drawDepth = transform[3].z;

	*command = cmd;

	return true;
}

bool TextRenderer::canRender() const
{
	return count && color.w > 0.0f;
}
