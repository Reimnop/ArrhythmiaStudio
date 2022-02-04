#include "SampleTextRenderer.h"

#include <glad/glad.h>

Shader* SampleTextRenderer::shader;

SampleTextRenderer::SampleTextRenderer(std::filesystem::path fontPath)
{
	if (!shader)
	{
		shader = new Shader("Assets/Shaders/text.vert", "Assets/Shaders/text.frag");
	}

	font = new Font(fontPath, "SampleFont");
	generator = new TextMeshGenerator(font);

	std::vector<TextVertex> vertices = generator->genMesh("Hello world!");
	count = vertices.size();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(TextVertex), vertices.data(), GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(TextVertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(TextVertex), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SampleTextRenderer::~SampleTextRenderer()
{
	delete font;
	delete generator;
}

void SampleTextRenderer::setText(std::string text)
{
	std::vector<TextVertex> vertices = generator->genMesh(text);
	count = vertices.size();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(TextVertex), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SampleTextRenderer::draw(glm::mat4 mvp)
{
	glBindVertexArray(vao);
	glUseProgram(shader->getHandle());

	glUniformMatrix4fv(0, 1, false, &mvp[0][0]);
	glBindTexture(GL_TEXTURE_2D, font->getAtlasTextureHandle());

	glDrawArrays(GL_TRIANGLES, 0, count);
}
