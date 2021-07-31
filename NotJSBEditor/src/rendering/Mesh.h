#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "VertexAttrib.h"

class Mesh {
public:
	glm::vec3* vertices;
	uint32_t* indices;

	int verticesCount;
	int indicesCount;

	Mesh(int verticesCount, glm::vec3* vertices, int indicesCount, uint32_t* indices);
	~Mesh();

	void setPerVertexData(int size, void* data, int attribCount, VertexAttrib* attribs);

	uint32_t getVao();
private:
	uint32_t vao;
	uint32_t vbo0;
	uint32_t vbo1;
	uint32_t ebo;
};