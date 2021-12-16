#pragma once

#include <vector>

#include "BatchedVertex.h"
#include "Drawer.h"
#include "../Mesh.h"
#include "../Shader.h"

class BatchedDrawer : public Drawer
{
public:
	void appendMesh(Mesh* mesh, glm::mat4 transform, glm::vec4 color);
	void draw(glm::mat4 cameraMatrix) override;
private:
	static inline Shader* shader;

	std::vector<BatchedVertex> vertices;
	std::vector<uint32_t> indices;
};