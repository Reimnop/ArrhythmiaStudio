#include "Sequence.h"

#include <cmath>

#include "Easing.h"

Sequence::Sequence()
{
	lastIndex = 0;
}

Sequence::Sequence(int count, Keyframe* keyframes)
{
	for (int i = 0; i < count; i++)
	{
		insertKeyframe(keyframes[i]);
	}

	lastIndex = 0;
}

Sequence::Sequence(json& j)
{
	fromJson(j);
}

void Sequence::loadKeyframes(std::vector<Keyframe>& keyframes)
{
	this->keyframes = keyframes;
	std::sort(this->keyframes.begin(), this->keyframes.end(), 
		[](Keyframe a, Keyframe b)
		{
			return a.time < b.time;
		});
}

void Sequence::insertKeyframe(Keyframe keyframe)
{
	keyframes.insert(std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
		[](Keyframe a, Keyframe b)
		{
			return a.time < b.time;
		}), keyframe);
}

void Sequence::eraseKeyframe(Keyframe keyframe)
{
	keyframes.erase(std::remove(keyframes.begin(), keyframes.end(), keyframe));
}

float Sequence::update(float time)
{
	// Bounds checking
	if (keyframes.empty())
	{
		return 0.0f;
	}

	if (keyframes.size() == 1)
	{
		return keyframes.front().value;
	}

	if (time < keyframes.front().time)
	{
		return keyframes.front().value;
	}

	if (time >= keyframes.back().time)
	{
		return keyframes.back().value;
	}

	// If time is not out of bounds, find left and right keyframes
	Keyframe left, right;
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

	return std::lerp(left.value, right.value, easedT);
}

void Sequence::fromJson(json& j)
{
	json::array_t arr = j;
	std::vector<Keyframe> keyframes;
	keyframes.reserve(arr.size());
	for (json kfJ : arr)
	{
		keyframes.emplace_back(kfJ);
	}
	loadKeyframes(keyframes);
}

json Sequence::toJson()
{
	json::array_t j;
	for (Keyframe kf : keyframes)
	{
		j.push_back(kf.toJson());
	}
	return j;
}
