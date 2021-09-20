#include "AnimationChannel.h"

AnimationChannel::AnimationChannel(AnimationChannelType type, int count, Keyframe* keyframes)
{
	this->type = type;
	sequence = new Sequence(count, keyframes);

	for (int i = 0; i < count; i++)
	{
		this->keyframes.push_back(keyframes[i]);
	}
}

AnimationChannel::AnimationChannel(nlohmann::json j)
{
	type = j["type"].get<AnimationChannelType>();

	sequence = new Sequence(0, nullptr);
	for (const nlohmann::json& kfJson : j["keyframes"])
	{
		insertKeyframe(Keyframe(kfJson));
	}
}

AnimationChannel::~AnimationChannel()
{
	delete sequence;
}

void AnimationChannel::insertKeyframe(const Keyframe& kf)
{
	keyframes.push_back(kf);
	sequence->insertKeyframe(kf);
}

void AnimationChannel::eraseKeyframe(const Keyframe& kf)
{
	const std::vector<Keyframe>::iterator it = std::remove(keyframes.begin(), keyframes.end(), kf);
	keyframes.erase(it);

	sequence->eraseKeyframe(kf);
}

float AnimationChannel::update(float time) const
{
	return sequence->update(time);
}

nlohmann::ordered_json AnimationChannel::toJson() const
{
	nlohmann::ordered_json j;
	j["type"] = type;

	j["keyframes"] = nlohmann::json::array();
	for (int i = 0; i < keyframes.size(); i++)
	{
		j["keyframes"].push_back(keyframes[i].toJson());
	}

	return j;
}
