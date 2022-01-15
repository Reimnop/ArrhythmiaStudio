#include "Font.h"

#include <glad/glad.h>

#include "msdf-atlas-gen/msdf-atlas-gen.h"

using namespace msdfgen;
using namespace msdf_atlas;

Font::Font(std::filesystem::path path)
{
	if (FreetypeHandle* ft = initializeFreetype())
	{
		if (FontHandle* font = loadFont(ft, path.generic_string().c_str()))
		{
			std::vector<GlyphGeometry> glyphs;

			FontGeometry fontGeometry(&glyphs);

			Charset charset;
			for (unicode_t i = 0x0000; i < 0xFFFF; i++)
			{
				charset.add(i);
			}

			fontGeometry.loadCharset(font, 1.0, charset);

			const double maxCornerAngle = 3.0;
			for (GlyphGeometry& glyph : glyphs)
				glyph.edgeColoring(&edgeColoringInkTrap, maxCornerAngle, 0);

			TightAtlasPacker packer;
			packer.setDimensionsConstraint(TightAtlasPacker::DimensionsConstraint::SQUARE);
			packer.setMinimumScale(24.0);
			packer.setPixelRange(2.0);
			packer.setMiterLimit(1.0);
			packer.pack(glyphs.data(), glyphs.size());
			int width = 0, height = 0;
			packer.getDimensions(width, height);

			ImmediateAtlasGenerator<
				float,
				4,
				mtsdfGenerator,
				BitmapAtlasStorage<byte, 4>
			> generator(width, height);

			GeneratorAttributes attributes;
			generator.setAttributes(attributes);
			generator.setThreadCount(4);

			generator.generate(glyphs.data(), glyphs.size());

			BitmapConstRef<byte, 4> atlasBitmap = generator.atlasStorage();

			glGenTextures(1, &texHandle);
			glBindTexture(GL_TEXTURE_2D, texHandle);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasBitmap.width, atlasBitmap.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasBitmap.pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);

			atlasInfo.size = 1.0f;
			atlasInfo.width = atlasBitmap.width;
			atlasInfo.height = atlasBitmap.height;

			const FontMetrics& fontMetrics = fontGeometry.getMetrics();
			metrics.lineHeight = fontMetrics.lineHeight;
			metrics.ascender = fontMetrics.ascenderY;
			metrics.descender = fontMetrics.descenderY;
			metrics.underlineY = fontMetrics.underlineY;
			metrics.underlineThickness = fontMetrics.underlineThickness;

			for (const GlyphGeometry& glyphGeometry : fontGeometry.getGlyphs())
			{
				Glyph glyph;
				glyph.unicode = glyphGeometry.getCodepoint();
				glyph.advance = glyphGeometry.getAdvance();

				double pl = 0.0f, pb = 0.0f, pr = 0.0f, pt = 0.0f, al = 0.0f, ab = 0.0f, ar = 0.0f, at = 0.0f;
				glyphGeometry.getQuadPlaneBounds(pl, pb, pr, pt);
				glyphGeometry.getQuadAtlasBounds(al, ab, ar, at);

				glyph.isEmpty = !((pl || pb || pr || pt) && (al || ab || ar || at));

				if (!glyph.isEmpty) 
				{
					Bounds planeBounds;
					planeBounds.left = pl;
					planeBounds.bottom = pb;
					planeBounds.right = pr;
					planeBounds.top = pt;

					Bounds atlasBounds;
					atlasBounds.left = al;
					atlasBounds.bottom = atlasBitmap.height - ab;
					atlasBounds.right = ar;
					atlasBounds.top = atlasBitmap.height - at;

					glyph.planeBounds = planeBounds;
					glyph.atlasBounds = atlasBounds;
				}

				this->glyphs.emplace(glyph.unicode, glyph);
			}

			for (const std::pair<std::pair<int, int>, double>& kernPair : fontGeometry.getKerning()) 
			{
				kerning.emplace(kernPair.first, kernPair.second);
			}

			destroyFont(font);
		}
		deinitializeFreetype(ft);
	}
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
