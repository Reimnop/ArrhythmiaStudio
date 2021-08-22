#pragma once

#include <nlohmann/json.hpp>

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
	ColorSlot(nlohmann::json j);
	~ColorSlot();

	void update(float time);
	nlohmann::ordered_json toJson() const;
private:
	static Shader* shader;
};
