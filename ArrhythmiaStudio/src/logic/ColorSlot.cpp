#include "ColorSlot.h"

ColorSlot::ColorSlot()
{
	ColorKeyframe kfWhite;
	kfWhite.time = 0.0f;
	kfWhite.color = Color(1.0f, 1.0f, 1.0f);
	sequence = ColorSequence(1, &kfWhite);
}

ColorSlot::ColorSlot(json& j)
{
	sequence = ColorSequence(j);
}

void ColorSlot::update(float t)
{
	color = sequence.update(t);
}

Color ColorSlot::getColor()
{
	return color;
}

json ColorSlot::toJson()
{
	return sequence.toJson();
}

void ColorSlot::fromJson(json& j)
{
	sequence.fromJson(j);
}
