#include "ColorIndexSequence.h"

#include <cmath>

ColorIndexSequence::ColorIndexSequence(Level* level)
{
	this->level = level;
	lastIndex = 0;
}

ColorIndexSequence::ColorIndexSequence(int count, ColorIndexKeyframe* keyframes, Level* level)
{
    this->level = level;
	for (int i = 0; i < count; i++)
	{
		insertKeyframe(keyframes[i]);
	}

	lastIndex = 0;
}

ColorIndexSequence::ColorIndexSequence(json& j, Level* level)
{
    this->level = level;
	fromJson(j);
}

void ColorIndexSequence::loadKeyframes(std::vector<ColorIndexKeyframe>& keyframes)
{
	this->keyframes = keyframes;
	std::sort(this->keyframes.begin(), this->keyframes.end(),
		[](ColorIndexKeyframe a, ColorIndexKeyframe b)
		{
			return a.time < b.time;
		});
}

void ColorIndexSequence::insertKeyframe(ColorIndexKeyframe keyframe)
{
	keyframes.insert(std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
		[](ColorIndexKeyframe a, ColorIndexKeyframe b)
		{
			return a.time < b.time;
		}), keyframe);
}

void ColorIndexSequence::eraseKeyframe(ColorIndexKeyframe keyframe)
{
	keyframes.erase(std::remove(keyframes.begin(), keyframes.end(), keyframe));
}

Color ColorIndexSequence::update(float time)
{
	// Bounds checking
	if (keyframes.empty())
	{
		return Color(1.0f, 1.0f, 1.0f);
	}

	if (keyframes.size() == 1)
	{
		return level->colorSlots[keyframes.front().value]->color;
	}

	if (time < keyframes.front().time)
	{
		return level->colorSlots[keyframes.front().value]->color;
	}

	if (time >= keyframes.back().time)
	{
		return level->colorSlots[keyframes.back().value]->color;
	}

	// If time is not out of bounds, find left and right keyframes
	ColorIndexKeyframe left, right;
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

	const Color leftColor = level->colorSlots[left.value]->color;
	const Color rightColor = level->colorSlots[right.value]->color;

	return Color(
		std::lerp(leftColor.r, rightColor.r, easedT),
		std::lerp(leftColor.g, rightColor.g, easedT),
		std::lerp(leftColor.b, rightColor.b, easedT));
}

void ColorIndexSequence::fromJson(json& j)
{
	json::array_t arr = j;
	std::vector<ColorIndexKeyframe> keyframes;
	keyframes.reserve(arr.size());
	for (json kfJ : arr)
	{
		keyframes.emplace_back(kfJ);
	}
	loadKeyframes(keyframes);
}

json ColorIndexSequence::toJson()
{
	json::array_t j;
	for (ColorIndexKeyframe kf : keyframes)
	{
		j.push_back(kf.toJson());
	}
	return j;
}
