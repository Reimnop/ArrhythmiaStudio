#include "BatchedDrawer.h"

void BatchedDrawer::appendMesh(Mesh* mesh, glm::mat4 transform, glm::vec4 color)
{
	uint32_t indexOffset = vertices.size();

	// Append vertices
	vertices.reserve(vertices.size() + mesh->vertices.size());

	for (glm::vec3 pos : mesh->vertices)
	{
		BatchedVertex vertex;
		vertex.position = glm::vec3(transform * glm::vec4(pos, 1.0f));
		vertex.color = color;
		vertices.push_back(vertex);
	}

	// Append indices
	indices.reserve(indices.size() + mesh->indices.size());

	for (uint32_t index : mesh->indices)
	{
		indices.push_back(indexOffset + index);
	}
}

void BatchedDrawer::draw(glm::mat4 cameraMatrix)
{
	// Create shader if not already created
	if (!shader) 
	{
		shader = new Shader("Assets/Shaders/unlit.vert", "Assets/Shaders/unlit.frag");
	}

	// Copy data to GPU buffer
	uint32_t vao, vbo, ebo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BatchedVertex), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(BatchedVertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(BatchedVertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Draw
	glBindVertexArray(vao);

	glUseProgram(shader->getHandle());
	glUniformMatrix4fv(0, 1, false, &cameraMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
