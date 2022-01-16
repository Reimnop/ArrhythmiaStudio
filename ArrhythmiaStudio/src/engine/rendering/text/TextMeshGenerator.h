#pragma once

#include "Font.h"
#include "TextVertex.h"

class TextMeshGenerator
{
public:
	TextMeshGenerator(Font* font);

	std::vector<TextVertex> genMesh(const std::wstring& text) const;
private:
	Font* font;

	std::vector<std::wstring> split(std::wstring in_pattern, const std::wstring& content) const;
};
