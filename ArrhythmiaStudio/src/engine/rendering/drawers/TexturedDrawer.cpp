#include "TexturedDrawer.h"
#include "../TexturedDrawData.h"
#include "glad/glad.h"

TexturedDrawer::TexturedDrawer(TexturedDrawData *drawData)
{
    transform = drawData->transform;
    color = drawData->color;
    texture = drawData->texture;
}

void TexturedDrawer::draw(glm::mat4 cameraMatrix)
{
    if (!shader)
    {
        init();
    }

    glm::mat4 mvp = cameraMatrix * transform;

    glUseProgram(shader->getHandle());
    glUniformMatrix4fv(0, 1, false, &mvp[0][0]);
    glUniform3f(1, color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TexturedDrawer::init()
{
    const TexturedVertex vertices[] = {
        TexturedVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
        TexturedVertex(0.5f, 0.5f, 0.0f, 1.0f, 1.0f),
        TexturedVertex(-0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
        TexturedVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
        TexturedVertex(0.5f, -0.5f, 0.0f, 1.0f, 1.0f),
        TexturedVertex(0.5f, 0.5f, 0.0f, 1.0f, 1.0f)
    };

    shader = new Shader("Assets/Shaders/textured.vert","Assets/Shaders/textured.frag");

    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(TexturedVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(TexturedVertex), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
