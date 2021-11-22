#pragma once

#include "RendererComponent.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

class MeshRenderer : public RendererComponent
{
public:
	MeshRenderer();
	~MeshRenderer() override = default;

	Mesh* mesh;
	Material* material;
	Shader* shader;

	float opacity = 1.0f;

	bool tryRender(glm::mat4 transform, RenderCommand** command) override;
private:
	// Check if rendering is possible
	bool canRender() const;
};
