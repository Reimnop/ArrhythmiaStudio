#pragma once

#include "DrawData.h"
#include "drawers/TexturedDrawer.h"

struct TexturedDrawData : DrawData
{
    glm::mat4 transform;
    glm::vec4 color;
    uint32_t texture;

    DrawDataType getType() const override
    {
        return DrawDataType_Textured;
    }

    Drawer* getDrawer() override
    {
        return new TexturedDrawer(this);
    }
};
