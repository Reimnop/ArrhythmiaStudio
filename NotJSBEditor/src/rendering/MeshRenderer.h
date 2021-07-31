#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "RendererComponent.h"
#include "Mesh.h"
#include "Material.h"
#include "DrawCommandType.h"

class MeshRenderer : public RendererComponent {
public:
	MeshRenderer();
	~MeshRenderer();

	Mesh* mesh;
	Material* material;

	bool render(InputDrawData input, OutputDrawData* output);
private:
	uint32_t uniformBuffers[1];

	// Check if rendering is possible
	bool canRender();
};