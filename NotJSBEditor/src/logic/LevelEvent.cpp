#include "LevelEvent.h"

LevelEvent::LevelEvent(LevelEventType type, int count, Keyframe* keyframes)
{
	this->type = type;
	sequence = new Sequence(count, keyframes);
}

LevelEvent::LevelEvent(nlohmann::json j)
{
	type = j["type"].get<LevelEventType>();
	sequence = new Sequence(j["keyframes"]);
}

LevelEvent::~LevelEvent()
{
	delete sequence;
}

float LevelEvent::update(float time)
{
	return sequence->update(time);
}

nlohmann::ordered_json LevelEvent::toJson()
{
	nlohmann::ordered_json j;
	j["type"] = type;
	j["keyframes"] = sequence->toJson();

	return j;
}
