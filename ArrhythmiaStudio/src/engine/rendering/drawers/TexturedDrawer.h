#pragma once

#include "Drawer.h"
#include "../Shader.h"

struct TexturedVertex
{
    glm::vec3 position;
    glm::vec2 uv;

    TexturedVertex(float x, float y, float z, float u, float v)
    {
        position = glm::vec3(x, y, z);
        uv = glm::vec2(u, v);
    }
};

struct TexturedDrawData;
class TexturedDrawer : public Drawer
{
public:
    TexturedDrawer(TexturedDrawData* drawData);
    ~TexturedDrawer() override = default;

    void draw(glm::mat4 cameraMatrix) override;
private:
    static inline Shader* shader;
    static inline uint32_t vao;

    static void init();

    glm::mat4 transform;
    glm::vec4 color;
    uint32_t texture;
};
