#include "TextDrawer.h"
#include "../TextDrawData.h"

#include "glad/glad.h"

TextDrawer::TextDrawer(TextDrawData* drawData)
{
	font = drawData->font;
	transform = drawData->transform;
	color = drawData->color;
	vao = drawData->vao;
	count = drawData->count;
}

void TextDrawer::draw(glm::mat4 cameraMatrix)
{
	if (!shader)
	{
		shader = new Shader("Assets/Shaders/text.vert", "Assets/Shaders/text.frag");
	}

	glm::mat4 mvp = cameraMatrix * transform;

	glUseProgram(shader->getHandle());
	glBindVertexArray(vao);

	glUniformMatrix4fv(0, 1, false, &mvp[0][0]);
	glUniform4f(1, color.x, color.y, color.z, color.w);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font->getAtlasTextureHandle());

	glDrawArrays(GL_TRIANGLES, 0, count);
}
