#include "ColorSequence.h"

#include "json.hpp"
#include "Easing.h"

ColorSequence::ColorSequence()
{
	lastIndex = 0;
}

ColorSequence::ColorSequence(int count, ColorKeyframe* keyframes)
{
	for (int i = 0; i < count; i++)
	{
		insertKeyframe(keyframes[i]);
	}

	lastIndex = 0;
}

ColorSequence::ColorSequence(json& j)
{
	fromJson(j);
}

void ColorSequence::loadKeyframes(std::vector<ColorKeyframe>& keyframes)
{
	this->keyframes = keyframes;
	std::sort(this->keyframes.begin(), this->keyframes.end(),
		[](ColorKeyframe a, ColorKeyframe b)
		{
			return a.time < b.time;
		});
}

void ColorSequence::insertKeyframe(ColorKeyframe keyframe)
{
	if (keyframes.empty())
	{
		keyframes.push_back(keyframe);
		return;
	}

	if (keyframes.back().time <= keyframe.time)
	{
		keyframes.push_back(keyframe);
		return;
	}

	const std::vector<ColorKeyframe>::iterator it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
																	[](ColorKeyframe a, ColorKeyframe b)
																	{
																	    return a.time < b.time;
																	});
	keyframes.insert(it, keyframe);
}

void ColorSequence::eraseKeyframe(ColorKeyframe keyframe)
{
	const std::vector<ColorKeyframe>::iterator it = std::remove(keyframes.begin(), keyframes.end(), keyframe);
	keyframes.erase(it);
}

Color ColorSequence::update(float time)
{
	// Bounds checking
	if (keyframes.empty())
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
	const EaseFunc ease = Easing::getEaseFunction(right.easing);
	const float t = (time - left.time) / (right.time - left.time);
	const float easedT = ease(t);

	const float r = std::lerp(left.color.r, right.color.r, easedT);
	const float g = std::lerp(left.color.g, right.color.g, easedT);
	const float b = std::lerp(left.color.b, right.color.b, easedT);
	return Color(r, g, b);
}

void ColorSequence::fromJson(json& j)
{
	json::array_t arr = j;
	std::vector<ColorKeyframe> keyframes;
	keyframes.reserve(arr.size());
	for (json kfJ : arr)
	{
		keyframes.emplace_back(kfJ);
	}
	loadKeyframes(keyframes);
}

json ColorSequence::toJson()
{
	json::array_t j;
	for (ColorKeyframe kf : keyframes)
	{
		j.push_back(kf.toJson());
	}
	return j;
}