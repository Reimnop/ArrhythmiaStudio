#pragma once

#include <glm/glm.hpp>

#include "animation/Color.h"
#include "animation/ColorChannel.h"
#include "../rendering/Shader.h"
#include "../rendering/Material.h"

class ColorSlot
{
public:
	ColorChannel* channel;
	Material* material;

	Color currentColor;

	static void init();

	ColorSlot(int count, ColorKeyframe* keyframes);
	~ColorSlot();

	void update(float time);
private:
	static Shader* shader;
};
