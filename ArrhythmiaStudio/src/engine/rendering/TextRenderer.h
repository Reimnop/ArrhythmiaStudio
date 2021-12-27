#pragma once

#include "RendererComponent.h"
#include "Material.h"
#include "text/TextMeshGenerator.h"

class TextRenderer : public RendererComponent
{
public:
	TextRenderer(Font* font);
	~TextRenderer() override;

	glm::vec4 color;

	void setText(const std::wstring& text);
	bool tryRender(glm::mat4 transform, RenderCommand** command) override;
private:
	Font* font;
	size_t count;

	uint32_t vao;
	uint32_t vbo;

	// Check if rendering is possible
	bool canRender() const;
};
