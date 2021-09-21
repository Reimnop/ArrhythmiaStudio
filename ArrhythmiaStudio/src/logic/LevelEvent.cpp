#include "LevelEvent.h"

LevelEvent::LevelEvent(LevelEventType type, int count, Keyframe* keyframes)
{
	this->type = type;
	sequence = new Sequence(count, keyframes);

	for (int i = 0; i < count; i++)
	{
		this->keyframes.push_back(keyframes[i]);
	}
}

LevelEvent::LevelEvent(nlohmann::json j)
{
	type = j["type"].get<LevelEventType>();

	sequence = new Sequence(0, nullptr);
	for (const nlohmann::json& kfJson : j["keyframes"])
	{
		insertKeyframe(Keyframe(kfJson));
	}
}

LevelEvent::~LevelEvent()
{
	delete sequence;
}

void LevelEvent::insertKeyframe(Keyframe keyframe)
{
	keyframes.push_back(keyframe);
	sequence->insertKeyframe(keyframe);
}

void LevelEvent::eraseKeyframe(Keyframe keyframe)
{
	const std::vector<Keyframe>::iterator it = std::remove(keyframes.begin(), keyframes.end(), keyframe);
	keyframes.erase(it);

	sequence->eraseKeyframe(keyframe);
}

float LevelEvent::update(float time) const
{
	return sequence->update(time);
}

nlohmann::ordered_json LevelEvent::toJson() const
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
