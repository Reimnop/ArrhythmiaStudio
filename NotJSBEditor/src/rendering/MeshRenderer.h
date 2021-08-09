#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "RendererComponent.h"
#include "Mesh.h"
#include "Material.h"
#include "DrawElementsCommand.h"
#include "DrawCommandType.h"

class MeshRenderer : public RendererComponent
{
public:
	MeshRenderer();
	~MeshRenderer() override;

	Mesh* mesh;
	Material* material;

	bool render(InputDrawData input, OutputDrawData* output) override;
private:
	uint32_t uniformBuffers[2];

	// Check if rendering is possible
	bool canRender();
};
