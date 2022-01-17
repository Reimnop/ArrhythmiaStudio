#pragma once

#include <unordered_map>
#include <filesystem>
#include <helper.h>

#include "ft2build.h"
#include "AtlasInfo.h"
#include "Glyph.h"
#include "Metrics.h"
#include "hb.h"
#include "freetype/freetype.h"

class Font
{
public:
	Font(std::filesystem::path path);
	~Font();

	std::filesystem::path path;

	AtlasInfo getInfo() const;
	Metrics getMetrics() const;
	uint32_t getAtlasTextureHandle() const;
	bool tryGetGlyph(int c, Glyph* out);
	float getKerning(int l, int r);
	hb_font_t* getHbFont();
private:
	AtlasInfo atlasInfo;
	Metrics metrics;
	std::unordered_map<int, Glyph> glyphs;
	std::unordered_map<std::pair<int, int>, float, pair_hash> kerning;

	uint32_t texHandle;
	hb_font_t* hbFont;

	FT_Library library;
	FT_Face ftFace;
};
