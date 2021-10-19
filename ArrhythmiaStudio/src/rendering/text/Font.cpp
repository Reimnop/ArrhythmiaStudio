#include "Font.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <glad/glad.h>

Font::Font(std::filesystem::path path)
{
	std::ifstream s(path, std::ios::binary);

	// read json
	int json_length = 0;
	s.read((char*)&json_length, 4);
	char* json_buf = new char[json_length + 1];
	s.read(json_buf, json_length);
	json_buf[json_length] = 0; // null termination

	nlohmann::json j = nlohmann::json::parse(json_buf);

	atlasInfo.size = j["atlas"]["size"].get<float>();
	atlasInfo.width = j["atlas"]["width"].get<int>();
	atlasInfo.height = j["atlas"]["height"].get<int>();

	metrics.lineHeight = j["metrics"]["lineHeight"].get<float>();
	metrics.ascender = j["metrics"]["ascender"].get<float>();
	metrics.descender = j["metrics"]["descender"].get<float>();
	metrics.underlineY = j["metrics"]["underlineY"].get<float>();
	metrics.underlineThickness = j["metrics"]["underlineThickness"].get<float>();

	for (const nlohmann::json& glyph_json : j["glyphs"])
	{
		Glyph glyph = Glyph();
		glyph.unicode = (wchar_t)glyph_json["unicode"].get<uint16_t>();
		glyph.advance = glyph_json["advance"].get<float>();

		if (glyph_json.contains("planeBounds") && glyph_json.contains("atlasBounds"))
		{
			Bounds planeBounds = Bounds();
			planeBounds.left = glyph_json["planeBounds"]["left"].get<float>();
			planeBounds.bottom = glyph_json["planeBounds"]["bottom"].get<float>();
			planeBounds.right = glyph_json["planeBounds"]["right"].get<float>();
			planeBounds.top = glyph_json["planeBounds"]["top"].get<float>();

			Bounds atlasBounds = Bounds();
			atlasBounds.left = glyph_json["atlasBounds"]["left"].get<float>();
			atlasBounds.bottom = glyph_json["atlasBounds"]["bottom"].get<float>();
			atlasBounds.right = glyph_json["atlasBounds"]["right"].get<float>();
			atlasBounds.top = glyph_json["atlasBounds"]["top"].get<float>();

			glyph.isEmpty = false;
			glyph.planeBounds = planeBounds;
			glyph.atlasBounds = atlasBounds;
		}
		else
		{
			glyph.isEmpty = true;
		}

		glyphs.emplace(glyph.unicode, glyph);
	}

	for (const nlohmann::json& kerning : j["kerning"])
	{
		wchar_t unicode1 = (wchar_t)kerning["unicode1"].get<uint16_t>();
		wchar_t unicode2 = (wchar_t)kerning["unicode2"].get<uint16_t>();
		float advance = kerning["advance"].get<uint16_t>();

		this->kerning.emplace(std::pair(unicode1, unicode2), advance);
	}

	// read atlas
	int width = 0, height = 0;
	s.read((char*)&width, 4);
	s.read((char*)&height, 4);
	void* data = malloc(width * height * 4);
	s.read((char*)data, width * height * 4);

	glGenTextures(1, &texHandle);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(data);
	delete[] json_buf;
	s.close();
}

Font::~Font()
{
	glDeleteTextures(1, &texHandle);
}

AtlasInfo Font::getInfo() const
{
	return atlasInfo;
}

Metrics Font::getMetrics() const
{
	return metrics;
}

uint32_t Font::getAtlasTextureHandle() const
{
	return texHandle;
}

bool Font::tryGetGlyph(wchar_t c, Glyph* out)
{
	if (glyphs.contains(c))
	{
		if (out) 
		{
			*out = glyphs[c];
		}
		return true;
	}
	return false;
}

float Font::getKerning(wchar_t l, wchar_t r)
{
	const std::pair p(l, r);
	if (kerning.contains(p))
	{
		return kerning[p];
	}
	return 0.0f;
}
