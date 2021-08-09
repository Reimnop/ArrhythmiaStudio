#include "ColorChannel.h"

ColorChannel::ColorChannel(int count, ColorKeyframe* keyframes)
{
	for (int i = 0; i < count; i++)
	{
		insertKeyframe(keyframes[i]);
	}

	lastIndex = 0;
}

ColorChannel::~ColorChannel()
{
	keyframes.clear();
	keyframes.shrink_to_fit();
}

void ColorChannel::insertKeyframe(ColorKeyframe keyframe)
{
	if (keyframes.size() == 0)
	{
		keyframes.push_back(keyframe);
		return;
	}

	if (keyframes.back().time <= keyframe.time)
	{
		keyframes.push_back(keyframe);
		return;
	}

	std::vector<ColorKeyframe>::iterator it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
		[](ColorKeyframe a, ColorKeyframe b)
		{
			return a.time < b.time;
		});
	keyframes.insert(it, keyframe);
}

Color ColorChannel::update(float time)
{
	// Bounds checking
	if (keyframes.size() == 0)
	{
		return Color();
	}

	if (keyframes.size() == 1)
	{
		return keyframes.front().color;
	}

	if (time < keyframes.front().time)
	{
		return keyframes.front().color;
	}

	if (time >= keyframes.back().time)
	{
		return keyframes.back().color;
	}

	// If time is not out of bounds, find left and right keyframes
	ColorKeyframe left, right;
	if (time >= keyframes[lastIndex].time && time < keyframes[lastIndex + 1].time)
	{
		left = keyframes[lastIndex];
		right = keyframes[lastIndex + 1];
	}
	else
	{
		if (time >= keyframes[lastIndex + 1].time)
		{
			while (lastIndex + 1 < keyframes.size() - 1 && time >= keyframes[lastIndex + 1].time)
			{
				lastIndex++;
			}
		}
		else
		{
			while (lastIndex > 0 && time < keyframes[lastIndex].time)
			{
				lastIndex--;
			}
		}

		left = keyframes[lastIndex];
		right = keyframes[lastIndex + 1];
	}

	// Apply easings
	float t = (time - left.time) / (right.time - left.time);
	float r = lerp(left.color.r, right.color.r, t);
	float g = lerp(left.color.g, right.color.g, t);
	float b = lerp(left.color.b, right.color.b, t);
	return Color(r, g, b);
}

float ColorChannel::lerp(float a, float b, float t)
{
	return (a * (1.0f - t)) + (b * t);
}
