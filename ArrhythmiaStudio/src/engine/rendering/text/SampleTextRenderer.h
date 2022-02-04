#pragma once

#include <filesystem>

#include "TextMeshGenerator.h"
#include "../Shader.h"

class SampleTextRenderer
{
public:
	SampleTextRenderer(std::filesystem::path fontPath);
	~SampleTextRenderer();

	void setText(std::string text);
	void draw(glm::mat4 mvp);
private:
	static Shader* shader;

	Font* font;
	TextMeshGenerator* generator;

	uint32_t vao;
	uint32_t vbo;
	size_t count;
};
