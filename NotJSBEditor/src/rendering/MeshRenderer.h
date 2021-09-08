#pragma once

#include "RendererComponent.h"
#include "Mesh.h"
#include "Material.h"

class MeshRenderer : public RendererComponent
{
public:
	MeshRenderer();
	~MeshRenderer() override;

	Mesh* mesh;
	Material* material;

	float opacity = 1.0f;

	bool render(InputDrawData input, OutputDrawData** output) override;
private:
	// Check if rendering is possible
	bool canRender() const;
};
