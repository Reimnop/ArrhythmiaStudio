#include "TypedLevelEvent.h"
#include "../Level.h"

#include "../factories/LevelEventFactory.h"

TypedLevelEvent::TypedLevelEvent(Level* level, std::string type)
{
	this->level = level;
	this->type = type;
	LevelEventInfo info = LevelEventFactory::getFromId(type);
	levelEvent = info.createFunction(level);
}

TypedLevelEvent::TypedLevelEvent(Level* level, json j)
{
	this->level = level;
	type = j["type"].get<std::string>();
	LevelEventInfo info = LevelEventFactory::getFromId(type);
	levelEvent = info.createFunction(level);
	levelEvent->readJson(j);
}

TypedLevelEvent::~TypedLevelEvent()
{
	delete levelEvent;
}

std::string TypedLevelEvent::getTitle()
{
	return levelEvent->getTitle();
}

void TypedLevelEvent::update(float time)
{
	levelEvent->update(time);
}

json TypedLevelEvent::toJson()
{
	json j;
	j["type"] = type;
	levelEvent->writeJson(j);
	return j;
}

void TypedLevelEvent::fromJson(json j)
{
	if (type != j["type"].get<std::string>())
	{
		throw std::runtime_error("Mismatch level event type!");
	}
	levelEvent->readJson(j);
}

void TypedLevelEvent::drawEditor()
{
	levelEvent->drawEditor();
	update(level->time);
}
