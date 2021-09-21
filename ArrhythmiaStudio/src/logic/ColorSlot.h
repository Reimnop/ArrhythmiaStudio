#pragma once

#include <nlohmann/json.hpp>

#include "animation/Color.h"
#include "animation/ColorSequence.h"
#include "../rendering/Shader.h"
#include "../rendering/Material.h"

class ColorSlot
{
public:
	std::vector<ColorKeyframe> keyframes;

	ColorSequence* sequence;
	Material* material;

	Color currentColor;

	static void init();

	ColorSlot(int count, ColorKeyframe* keyframes);
	ColorSlot(nlohmann::json j);
	~ColorSlot();

	void insertKeyframe(ColorKeyframe kf);
	void eraseKeyframe(ColorKeyframe kf);

	void update(float time);
	nlohmann::json toJson() const;
private:
	static Shader* shader;
};
