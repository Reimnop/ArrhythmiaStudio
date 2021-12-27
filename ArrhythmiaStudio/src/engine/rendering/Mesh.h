#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "VertexAttrib.h"

class Mesh
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;

	Mesh(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices);
	~Mesh();

	void setPerVertexData(int size, void* data, int attribCount, VertexAttrib* attribs) const;

	uint32_t getVao() const;
private:
	uint32_t vao;
	uint32_t vbo0;
	uint32_t vbo1;
	uint32_t ebo;
};
