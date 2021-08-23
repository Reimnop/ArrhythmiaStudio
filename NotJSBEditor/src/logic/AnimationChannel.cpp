#include "AnimationChannel.h"

AnimationChannel::AnimationChannel(AnimationChannelType type, int count, Keyframe* keyframes)
{
	this->type = type;
	sequence = new Sequence(count, keyframes);
}

AnimationChannel::AnimationChannel(nlohmann::json j)
{
	type = j["type"].get<AnimationChannelType>();
	sequence = new Sequence(j["keyframes"]);
}

AnimationChannel::~AnimationChannel()
{
	delete sequence;
}

float AnimationChannel::update(float time)
{
	return sequence->update(time);
}

nlohmann::ordered_json AnimationChannel::toJson()
{
	nlohmann::ordered_json j;
	j["type"] = type;
	j["keyframes"] = sequence->toJson();

	return j;
}
