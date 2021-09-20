#include "Sequence.h"

#include "Easing.h"

Sequence::Sequence(int count, Keyframe* keyframes)
{
	for (int i = 0; i < count; i++)
	{
		insertKeyframe(keyframes[i]);
	}

	lastIndex = 0;
}

void Sequence::insertKeyframe(Keyframe keyframe)
{
	keyframe.evaluateValue();

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

	const std::vector<Keyframe>::iterator it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
																[](Keyframe a, Keyframe b)
																{
																    return a.time < b.time;
																});
	keyframes.insert(it, keyframe);
}

void Sequence::eraseKeyframe(Keyframe keyframe)
{
	const std::vector<Keyframe>::iterator it = std::remove(keyframes.begin(), keyframes.end(), keyframe);
	keyframes.erase(it);
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
		return keyframes.front().evaluatedValue;
	}

	if (time < keyframes.front().time)
	{
		return keyframes.front().evaluatedValue;
	}

	if (time >= keyframes.back().time)
	{
		return keyframes.back().evaluatedValue;
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

	return std::lerp(left.evaluatedValue, right.evaluatedValue, easedT);
}