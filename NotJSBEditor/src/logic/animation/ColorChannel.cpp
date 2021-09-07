#include "ColorChannel.h"

#include "Easing.h"

ColorChannel::ColorChannel(int count, ColorKeyframe* keyframes)
{
	for (int i = 0; i < count; i++)
	{
		insertKeyframe(keyframes[i]);
	}

	lastIndex = 0;
}

ColorChannel::ColorChannel(nlohmann::json j)
{
	nlohmann::json::array_t keyframes = j["keyframes"].get<nlohmann::json::array_t>();
	for (int i = 0; i < keyframes.size(); i++)
	{
		insertKeyframe(ColorKeyframe(keyframes[i]));
	}
}

void ColorChannel::insertKeyframe(ColorKeyframe keyframe)
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

nlohmann::ordered_json ColorChannel::toJson()
{
	nlohmann::ordered_json j;
	j["keyframes"] = nlohmann::json::array();
	for (int i = 0; i < keyframes.size(); i++)
	{
		j["keyframes"][i] = keyframes[i].toJson();
	}

	return j;
}
