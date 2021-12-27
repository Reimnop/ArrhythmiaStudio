#pragma once

#include "text/Font.h"
#include "drawers/TextDrawer.h"
#include "DrawData.h"

struct TextDrawData : DrawData
{
	uint32_t vao;
	Font* font;
	uint32_t count;
	glm::mat4 transform;
	glm::vec4 color;

	DrawDataType getType() const override
	{
		return DrawDataType_Text;
	}

	Drawer* getDrawer() override
	{
		return new TextDrawer(this);
	}
};
