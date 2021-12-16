#pragma once

#include "RendererComponent.h"
#include "Material.h"
#include "../../engine/rendering/Shader.h"
#include "text/TextMeshGenerator.h"

class TextRenderer : public RendererComponent
{
public:
	TextRenderer(Font* font);
	~TextRenderer() override;

	Material* material;

	float opacity = 1.0f;

	void setText(std::string text);
	bool tryRender(glm::mat4 transform, RenderCommand** command) override;
private:
	static Shader* shader;

	Font* font;
	size_t count;

	uint32_t vao;
	uint32_t vbo;

	// Check if rendering is possible
	bool canRender() const;
};
