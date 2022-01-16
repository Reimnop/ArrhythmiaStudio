#pragma once

#include "Bounds.h"

struct Glyph
{
	int index;
	float advance;
	bool isEmpty;
	Bounds planeBounds;
	Bounds atlasBounds;
};
