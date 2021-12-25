#include "Level.h"

#include <fstream>

#include "LevelObject.h"
#include "factories/LevelEventFactory.h"
#include "object_behaviours/LevelObjectBehaviour.h"

Level::Level(std::string name, path audioPath, path levelDir)
{
	this->levelDir = levelDir;
	this->name = name;

	// Copy audio file to level directory
	copy_file(audioPath, levelDir / "audio.ogg");

	clip = new AudioClip(levelDir / "audio.ogg");
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
	this->levelDir = levelDir;

	clip = new AudioClip(levelDir / "audio.ogg");
	levelLength = clip->getLength();

	std::ifstream i(levelDir / "level.aslv");
	json j;
	i >> j;

	name = j["name"].get<std::string>();
	bpm = j["bpm"].get<float>();
	offset = j["offset"].get<float>();

	json::array_t objArr = j["objects"];
	for (json objJ : objArr)
	{
		LevelObject* obj = new LevelObject(objJ);
		insertObject(obj);
		insertActivateList(obj);
		insertDeactivateList(obj);
	}
	recalculateObjectsState();

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

	for (auto &[id, obj] : levelEvents)
	{
		if (!obj)
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
	for (auto &[id, obj] : levelObjects)
	{
		delete obj;
	}

	for (auto &[id, levelEvent] : levelEvents)
	{
		delete levelEvent;
	}

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

	if (time > lastTime)
	{
		updateForward();
	}
	else if (time < lastTime)
	{
		updateReverse();
	}

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

	// Update objects
	for (LevelObject* obj : aliveObjects)
	{
		obj->update(time);
	}

	lastTime = time;
}

void Level::addObject(LevelObject* object)
{
	insertObject(object);
	insertActivateList(object);
	insertDeactivateList(object);
	recalculateObjectsState();
}

void Level::deleteObject(LevelObject* object)
{
	removeObject(object);
	removeActivateList(object);
	removeDeactivateList(object);
	recalculateObjectsState();
}

void Level::insertObject(LevelObject* object)
{
	object->level = this;
	levelObjects.emplace(object->id, object);
}

void Level::removeObject(LevelObject* object)
{
	object->level = nullptr;
	levelObjects.erase(object->id);
}

void Level::insertActivateList(LevelObject* object)
{
	activateList.insert(
		std::upper_bound(activateList.begin(), activateList.end(), object,
			[](LevelObject* a, LevelObject* b)
			{
				return a->startTime < b->startTime;
			}), object);
}

void Level::insertDeactivateList(LevelObject* object)
{
	deactivateList.insert(
		std::upper_bound(deactivateList.begin(), deactivateList.end(), object,
			[](LevelObject* a, LevelObject* b)
			{
				return a->endTime < b->endTime;
			}), object);
}

void Level::removeActivateList(LevelObject* object)
{
	activateList.erase(std::remove(activateList.begin(), activateList.end(), object));
}

void Level::removeDeactivateList(LevelObject* object)
{
	deactivateList.erase(std::remove(deactivateList.begin(), deactivateList.end(), object));
}

void Level::recalculateObjectsState()
{
	std::unordered_map<LevelObject*, int> activeTable;
	aliveObjects.clear();
	activateIndex = 0;
	deactivateIndex = 0;

	for (auto &[id, obj] : levelObjects)
	{
		activeTable[obj] = 0;
	}

	for (LevelObject* object : activateList)
	{
		if (object->startTime > time)
		{
			break;
		}

		activateIndex++;
		activeTable[object]++;
	}

	for (LevelObject* object : deactivateList)
	{
		if (object->endTime > time)
		{
			break;
		}

		deactivateIndex++;
		activeTable[object]--;
	}

	for (auto &[obj, activeIndex] : activeTable)
	{
		if (activeIndex % 2)
		{
			obj->node->setActive(true);
			aliveObjects.insert(obj);
		}
		else
		{
			obj->node->setActive(false);
		}
	}
}

void Level::updateForward()
{
	while (activateIndex < activateList.size() && time >= activateList[activateIndex]->startTime)
	{
		LevelObject* obj = activateList[activateIndex];
		obj->node->setActive(true);
		aliveObjects.insert(obj);
		activateIndex++;
	}

	while (deactivateIndex < deactivateList.size() && time >= deactivateList[deactivateIndex]->endTime)
	{
		LevelObject* obj = deactivateList[deactivateIndex];
		obj->node->setActive(false);
		aliveObjects.erase(obj);
		deactivateIndex++;
	}
}

void Level::updateReverse()
{
	while (deactivateIndex - 1 >= 0 && time < deactivateList[deactivateIndex - 1]->endTime)
	{
		LevelObject* obj = deactivateList[deactivateIndex - 1];
		obj->node->setActive(true);
		aliveObjects.insert(obj);
		deactivateIndex--;
	}

	while (activateIndex - 1 >= 0 && time < activateList[activateIndex - 1]->startTime)
	{
		LevelObject* obj = activateList[activateIndex - 1];
		obj->node->setActive(false);
		aliveObjects.erase(obj);
		activateIndex--;
	}
}

json Level::toJson()
{
	json j;
	j["name"] = name;
	j["bpm"] = bpm;
	j["offset"] = offset;
	json::array_t objArr;
	objArr.reserve(levelObjects.size());
	for (auto &[id, obj] : levelObjects)
	{
		objArr.push_back(obj->toJson());
	}
	j["objects"] = objArr;
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
