#pragma once

#include "Drawer.h"
#include "../text/Font.h"
#include "../Shader.h"

class TextDrawData;
class TextDrawer : public Drawer
{
public:
	TextDrawer(TextDrawData* drawData);
	~TextDrawer() override = default;

	void draw(glm::mat4 cameraMatrix) override;
private:
	static inline Shader* shader;

	Font* font;

	glm::mat4 transform;
	glm::vec4 color;

	uint32_t vao;
	uint32_t count;
};
