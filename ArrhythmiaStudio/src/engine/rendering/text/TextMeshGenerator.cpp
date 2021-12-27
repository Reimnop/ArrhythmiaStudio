#include "TextMeshGenerator.h"

#include <regex>

TextMeshGenerator::TextMeshGenerator(Font* font)
{
	this->font = font;
}

std::vector<TextVertex> TextMeshGenerator::genMesh(const std::wstring& text) const
{
	const AtlasInfo& info = font->getInfo();
	const Metrics& metrics = font->getMetrics();

	const size_t count = getVerticesCount(text);
	if (!count)
	{
		return std::vector<TextVertex>();
	}

	std::vector<TextVertex> vertices;
	vertices.reserve(count);

	std::vector<std::wstring> lines = split(L"\r|\n|\r\n", text);

	size_t vert_offset = 0;
	float y_offset = (lines.size() - 1) * metrics.lineHeight * 0.5f;
	for (const std::wstring& line : lines)
	{
		float x_offset = -getLineWidth(line) * 0.5f;

		for (int i = 0; i < line.size(); i++)
		{
			wchar_t c = line[i];

			Glyph g;
			if (!font->tryGetGlyph(c, &g))
			{
				font->tryGetGlyph(L'?', &g);
			}

			if (!g.isEmpty)
			{
				const Bounds& planeBounds = g.planeBounds;
				const Bounds& atlasBounds = g.atlasBounds;

				vertices.emplace_back(x_offset + planeBounds.right, y_offset + planeBounds.top, 0.0f, atlasBounds.right / info.width, (info.height - atlasBounds.top) / info.height);
				vertices.emplace_back(x_offset + planeBounds.left, y_offset + planeBounds.top, 0.0f, atlasBounds.left / info.width, (info.height - atlasBounds.top) / info.height);
				vertices.emplace_back(x_offset + planeBounds.left, y_offset + planeBounds.bottom, 0.0f, atlasBounds.left / info.width, (info.height - atlasBounds.bottom) / info.height);
				vertices.emplace_back(x_offset + planeBounds.right, y_offset + planeBounds.top, 0.0f, atlasBounds.right / info.width, (info.height - atlasBounds.top) / info.height);
				vertices.emplace_back(x_offset + planeBounds.left, y_offset + planeBounds.bottom, 0.0f, atlasBounds.left / info.width, (info.height - atlasBounds.bottom) / info.height);
				vertices.emplace_back(x_offset + planeBounds.right, y_offset + planeBounds.bottom, 0.0f, atlasBounds.right / info.width, (info.height - atlasBounds.bottom) / info.height);

				vert_offset += 6;
			}

			x_offset += g.advance;
			if (i < line.size() - 1)
			{
				x_offset += font->getKerning(line[i], line[i + 1]);
			}
		}
		
		y_offset -= metrics.lineHeight;
	}

	return vertices;
}

std::vector<std::wstring> TextMeshGenerator::split(std::wstring in_pattern, const std::wstring& content) const
{
	std::vector<std::wstring> split_content;

	std::wregex pattern(in_pattern);
	std::copy(std::wsregex_token_iterator(content.begin(), content.end(), pattern, -1), std::wsregex_token_iterator(), std::back_inserter(split_content));
	return split_content;
}

size_t TextMeshGenerator::getVerticesCount(const std::wstring& text) const
{
	size_t res = 0;
	for (const char c : text)
	{
		if (c != '\r' && c != '\n')
		{
			Glyph g;
			if (!font->tryGetGlyph(c, &g))
			{
				font->tryGetGlyph(L'?', &g);
			}

			if (!g.isEmpty)
			{
				res += 6;
			}
		}
	}

	return res;
}

float TextMeshGenerator::getLineWidth(const std::wstring& text) const
{
	float res = 0.0f;
	for (int i = 0; i < text.size(); i++)
	{
		wchar_t c = text[i];
		if (c != '\r' && c != '\n')
		{
			Glyph g;
			if (!font->tryGetGlyph(c, &g))
			{
				font->tryGetGlyph((wchar_t)'?', &g);
			}

			res += g.advance;
			if (i < text.size() - 1)
			{
				res += font->getKerning(text[i], text[i + 1]);
			}
		}
	}

	return res;
}
