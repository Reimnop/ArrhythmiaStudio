#pragma once

#include "Font.h"
#include "TextVertex.h"

class TextMeshGenerator
{
public:
	TextMeshGenerator(Font* font);

	std::vector<TextVertex> genMesh(const std::string& text) const;
private:
	Font* font;

	std::vector<std::string> split(std::string in_pattern, const std::string& content) const;
};
