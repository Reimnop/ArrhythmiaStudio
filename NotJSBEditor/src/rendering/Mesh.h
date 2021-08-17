#pragma once

#include <glm/glm.hpp>

#include "VertexAttrib.h"

class Mesh
{
public:
	glm::vec3* vertices;
	uint32_t* indices;

	int verticesCount;
	int indicesCount;

	Mesh(int verticesCount, int indicesCount, glm::vec3* vertices, uint32_t* indices);
	~Mesh();

	void setPerVertexData(int size, void* data, int attribCount, VertexAttrib* attribs) const;

	uint32_t getVao() const;
private:
	uint32_t vao;
	uint32_t vbo0;
	uint32_t vbo1;
	uint32_t ebo;
};
