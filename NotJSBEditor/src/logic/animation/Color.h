#pragma once

struct Color
{
	float r, g, b;

	Color()
	{
		this->r = 0.0f;
		this->g = 0.0f;
		this->b = 0.0f;
	}

	Color(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	bool operator==(Color b) const
	{
		return this->r == b.r && this->g == b.g && this->b == b.b;
	}
};
