#pragma once

#include <span>

#include "Font.h"
#include "TextVertex.h"

class TextMeshGenerator
{
public:
	TextMeshGenerator(Font* font);

	TextVertex* genMesh(const std::wstring& text, size_t* verticesCount) const;
private:
	Font* font;

	std::vector<std::wstring> split(std::wstring in_pattern, const std::wstring& content) const;
	size_t getVerticesCount(const std::wstring& text) const;
	float getLineWidth(const std::wstring& text) const;
};
