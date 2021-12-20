#pragma once
#include "animation/ColorSequence.h"

class ColorSlot
{
public:
	ColorSequence sequence;
	Color color;

	ColorSlot();
	ColorSlot(json& j);

	void update(float t);
	Color getColor();

	json toJson();
	void fromJson(json& j);
};
