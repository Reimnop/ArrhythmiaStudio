#include "Level.h"

#include <fstream>

#include "LevelObject.h"
#include "factories/LevelEventFactory.h"
#include "object_behaviours/LevelObjectBehaviour.h"

Level::Level(std::string name, path songPath, path levelDir)
{
	if (inst) 
	{
		return;
	}
	inst = this;

	this->levelDir = levelDir;
	this->name = name;

	// Copy audio file to level directory
	copy_file(songPath, levelDir / "song.ogg");

	clip = new AudioClip(levelDir / "song.ogg");
	levelLength = clip->getLength();

	for (std::string id : LevelEventFactory::getEventIds())
	{
		levelEvents.emplace(id, new TypedLevelEvent(this, id));
	}

	for (int i = 0; i < 20; i++)
	{
		colorSlots.push_back(new ColorSlot());
	}

	std::ofstream o(levelDir / "level.aslv");
	o << toJson();
}

Level::Level(path levelDir)
{
	if (inst)
	{
		return;
	}
	inst = this;

	this->levelDir = levelDir;

	clip = new AudioClip(levelDir / "song.ogg");
	levelLength = clip->getLength();

	std::ifstream i(levelDir / "level.aslv");
	json j;
	i >> j;

	name = j["name"].get<std::string>();
	bpm = j["bpm"].get<float>();
	offset = j["offset"].get<float>();

	json::array_t objsArr = j["objects"];
	spawner = new ObjectSpawner(objsArr);

	// Prepare level events map
	for (std::string id : LevelEventFactory::getEventIds())
	{
		levelEvents[id] = nullptr;
	}

	json::array_t eventsArr = j["events"];
	for (json eventJ : eventsArr)
	{
		TypedLevelEvent* levelEvent = new TypedLevelEvent(this, eventJ);

		if (levelEvents[levelEvent->getType()])
		{
			LOG4CXX_WARN(logger, "Duplicate level event, skipping! Duplicated value: " << levelEvent->getType().c_str());
			delete levelEvent;
		}
		else 
		{
			levelEvents[levelEvent->getType()] = levelEvent;
		}
	}

	for (auto &[id, levelEvent] : levelEvents)
	{
		if (!levelEvent)
		{
			LOG4CXX_WARN(logger, "Missing level event, creating new! Missing value: " << id.c_str());
			levelEvents[id] = new TypedLevelEvent(this, id);
		}
	}

	json::array_t colorSlotsArr = j["color_slots"];
	for (json colorSlotJ : colorSlotsArr)
	{
		colorSlots.push_back(new ColorSlot(colorSlotJ));
	}
}

Level::~Level()
{
	for (auto &[id, levelEvent] : levelEvents)
	{
		delete levelEvent;
	}

	delete spawner;

	delete clip;
}

void Level::seek(float t)
{
	time = t;
	clip->pause();
	clip->seek(t);
	update();
}

void Level::update()
{
	time = clip->getPosition();

	// Update events
	for (auto& [type, levelEvent] : levelEvents)
	{
		levelEvent->update(time);
	}

	// Update theme
	for (ColorSlot* slot : colorSlots)
	{
		slot->update(time);
	}

	spawner->update(time);
}

json Level::toJson()
{
	json j;
	j["name"] = name;
	j["bpm"] = bpm;
	j["offset"] = offset;
	j["objects"] = spawner->toJson();
	json::array_t eventArr;
	eventArr.reserve(levelEvents.size());
	for (auto &[type, levelEvent] : levelEvents)
	{
		eventArr.push_back(levelEvent->toJson());
	}
	j["events"] = eventArr;
	json::array_t colorSlotArr;
	colorSlotArr.reserve(colorSlots.size());
	for (ColorSlot* slot : colorSlots)
	{
		colorSlotArr.push_back(slot->toJson());
	}
	j["color_slots"] = colorSlotArr;
	return j;
}

void Level::save()
{
	std::ofstream o(levelDir / "level.aslv");
	o << toJson();
}
