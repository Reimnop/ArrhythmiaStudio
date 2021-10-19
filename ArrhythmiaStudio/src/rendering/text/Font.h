#pragma once

#include <unordered_map>
#include <filesystem>
#include <helper.h>

#include "AtlasInfo.h"
#include "Glyph.h"
#include "Metrics.h"

class Font
{
public:
	Font(std::filesystem::path path);
	~Font();

	AtlasInfo getInfo() const;
	Metrics getMetrics() const;
	uint32_t getAtlasTextureHandle() const;
	bool tryGetGlyph(wchar_t c, Glyph* out);
	float getKerning(wchar_t l, wchar_t r);
private:
	AtlasInfo atlasInfo;
	Metrics metrics;
	std::unordered_map<wchar_t, Glyph> glyphs;
	std::unordered_map<std::pair<wchar_t, wchar_t>, float, pair_hash> kerning;

	uint32_t texHandle;
};
