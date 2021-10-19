#pragma once

#include "Bounds.h"

struct Glyph
{
	wchar_t unicode;
	float advance;
	bool isEmpty;
	Bounds planeBounds;
	Bounds atlasBounds;
};
