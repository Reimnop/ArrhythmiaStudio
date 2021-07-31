#include "Mesh.h"

Mesh::Mesh(int verticesCount, glm::vec3* vertices, int indicesCount, uint32_t* indices) {
	this->verticesCount = verticesCount;
	this->indicesCount = indicesCount;
	this->vertices = vertices;
	this->indices = indices;

	vbo1 = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo0);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	// Location 0 is always position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), nullptr);
}

Mesh::~Mesh() {
	delete[] vertices;
	delete[] indices;

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo0);
	glDeleteBuffers(1, &vbo1);
	glDeleteBuffers(1, &ebo);
}

void Mesh::setPerVertexData(int size, void* data, int attribCount, VertexAttrib* attribs) {
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	int offset = 0;
	for (int i = 0; i < attribCount; i++)
	{
		VertexAttrib attrib = attribs[i];

		glEnableVertexAttribArray(i + 1);
		glVertexAttribPointer(i + 1, attrib.size, attrib.pointerType, attrib.normalized, 0, (void*)offset);

		offset += attrib.sizeInBytes;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint32_t Mesh::getVao() {
	return vao;
}