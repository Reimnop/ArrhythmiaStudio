#include "TextMeshGenerator.h"

#include "hb.h"

#include <regex>

TextMeshGenerator::TextMeshGenerator(Font* font)
{
	this->font = font;
}

std::vector<TextVertex> TextMeshGenerator::genMesh(const std::string& text) const
{
	std::vector<std::string> lines = split("\r|\n|\r\n", text);
	std::vector<TextVertex> vertices;

	AtlasInfo info = font->getInfo();
	Metrics metrics = font->getMetrics();

	float line_y = lines.size() * metrics.lineHeight / 2.0f;
	for (std::string& line : lines)
	{
		line_y -= metrics.lineHeight;

		hb_buffer_t* buf = hb_buffer_create();
		hb_buffer_add_utf8(buf, line.c_str(), -1, 0, -1);
		hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
		hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
		hb_buffer_set_language(buf, hb_language_from_string("en", -1));

		hb_shape(font->getHbFont(), buf, nullptr, 0);

		unsigned int glyph_count;
		hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
		hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

		std::vector<TextVertex> lineVertices;

		hb_position_t cursor_x = 0;
		hb_position_t cursor_y = 0;
		for (int i = 0; i < glyph_count; i++) 
		{
			hb_codepoint_t glyphid = glyph_info[i].codepoint;
			hb_position_t x_offset = glyph_pos[i].x_offset;
			hb_position_t y_offset = glyph_pos[i].y_offset;
			hb_position_t x_advance = glyph_pos[i].x_advance;
			hb_position_t y_advance = glyph_pos[i].y_advance;

			Glyph glyph;
			font->tryGetGlyph(glyphid, &glyph);

			Bounds planeBounds = glyph.planeBounds;
			Bounds atlasBounds = glyph.atlasBounds;

			if (!glyph.isEmpty)
			{
				float ws_x = cursor_x + x_offset;
				float ws_y = cursor_y + y_offset;

				ws_x /= Font::FONT_SIZE;
				ws_y /= Font::FONT_SIZE;
				ws_y += line_y;

				lineVertices.emplace_back(ws_x + planeBounds.right, ws_y + planeBounds.top, 0.0f, atlasBounds.right / info.width, (info.height - atlasBounds.top) / info.height);
				lineVertices.emplace_back(ws_x + planeBounds.left, ws_y + planeBounds.top, 0.0f, atlasBounds.left / info.width, (info.height - atlasBounds.top) / info.height);
				lineVertices.emplace_back(ws_x + planeBounds.left, ws_y + planeBounds.bottom, 0.0f, atlasBounds.left / info.width, (info.height - atlasBounds.bottom) / info.height);
				lineVertices.emplace_back(ws_x + planeBounds.right, ws_y + planeBounds.top, 0.0f, atlasBounds.right / info.width, (info.height - atlasBounds.top) / info.height);
				lineVertices.emplace_back(ws_x + planeBounds.left, ws_y + planeBounds.bottom, 0.0f, atlasBounds.left / info.width, (info.height - atlasBounds.bottom) / info.height);
				lineVertices.emplace_back(ws_x + planeBounds.right, ws_y + planeBounds.bottom, 0.0f, atlasBounds.right / info.width, (info.height - atlasBounds.bottom) / info.height);
			}

			cursor_x += x_advance;
			cursor_y += y_advance;
		}

		// Readjust to center
		for (TextVertex& vertex : lineVertices)
		{
			vertex.position.x -= cursor_x / (Font::FONT_SIZE * 2.0f);
		}

		vertices.insert(vertices.end(), lineVertices.begin(), lineVertices.end());

		hb_buffer_destroy(buf);
	}

	return vertices;
}

std::vector<std::string> TextMeshGenerator::split(std::string in_pattern, const std::string& content) const
{
	std::vector<std::string> split_content;

	std::regex pattern(in_pattern);
	std::copy(std::sregex_token_iterator(content.begin(), content.end(), pattern, -1), std::sregex_token_iterator(), std::back_inserter(split_content));
	return split_content;
}