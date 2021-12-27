#pragma once

#include <vector>

#include "BatchedVertex.h"
#include "Drawer.h"
#include "../Mesh.h"
#include "../Shader.h"

// Please, do not use this class as it is extremely specialized and will break if used anywhere else
class BatchedDrawer : public Drawer
{
public:
	BatchedDrawer();
	~BatchedDrawer() override = default;

	void appendMesh(Mesh* mesh, glm::mat4 transform, glm::vec4 color);
	void draw(glm::mat4 cameraMatrix) override;

	static void clearBatches();
private:
	struct BatchMeshInfo
	{
		Mesh* mesh;
		glm::mat4 transform;
		glm::vec4 color;
	};

	static inline Shader* shader;
	static inline std::vector<BatchedVertex> vertices;
	static inline std::vector<uint32_t> indices;

	static inline std::vector<BatchMeshInfo> batchedMeshes;

	int begin;
	int end;
};