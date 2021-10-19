#pragma once

#include <glm/glm.hpp>

struct TextVertex
{
	glm::vec3 position;
	glm::vec2 uv;

	TextVertex()
	{
		position = glm::vec3();
		uv = glm::vec2();
	}

	TextVertex(float x, float y, float z, float u, float v)
	{
		position = glm::vec3(x, y, z);
		uv = glm::vec2(u, v);
	}
};