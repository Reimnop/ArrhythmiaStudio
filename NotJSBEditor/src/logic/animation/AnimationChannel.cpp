#include "AnimationChannel.h"

AnimationChannel::AnimationChannel(AnimationChannelType channelType, int count, Keyframe* keyframes)
{
	for (int i = 0; i < count; i++)
	{
		insertKeyframe(keyframes[i]);
	}

	type = channelType;
	lastIndex = 0;
}

AnimationChannel::AnimationChannel(nlohmann::json j)
{
	type = j["type"].get<AnimationChannelType>();

	nlohmann::json::array_t arr = j["keyframes"].get<nlohmann::json::array_t>();
	for (int i = 0; i < arr.size(); i++)
	{
		insertKeyframe(Keyframe(arr[i]));
	}
}

AnimationChannel::~AnimationChannel()
{
	keyframes.clear();
	keyframes.shrink_to_fit();
}

void AnimationChannel::insertKeyframe(Keyframe keyframe)
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

	std::vector<Keyframe>::iterator it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
	                                                      [](Keyframe a, Keyframe b)
	                                                      {
		                                                      return a.time < b.time;
	                                                      });
	keyframes.insert(it, keyframe);
}

float AnimationChannel::update(float time)
{
	// Bounds checking
	if (keyframes.size() == 0)
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
	float t = (time - left.time) / (right.time - left.time);
	return lerp(left.value, right.value, t);
}

nlohmann::ordered_json AnimationChannel::toJson()
{
	nlohmann::ordered_json j;
	j["type"] = type;

	j["keyframes"] = nlohmann::json::array();
	for (int i = 0; i < keyframes.size(); i++)
	{
		j["keyframes"][i] = keyframes[i].toJson();
	}

	return j;
}

float AnimationChannel::lerp(float a, float b, float t)
{
	return (a * (1.0f - t)) + (b * t);
}
