#include "Level.h"

#include <fstream>

#include "LevelObject.h"
#include "editor_windows/Themes.h"
#include "factories/LevelEventFactory.h"
#include "level_events/AnimateableLevelEvent.h"
#include "object_behaviours/AnimateableObjectBehaviour.h"
#include "object_behaviours/AnimateableColoredObjectBehaviour.h"
#include "../engine/Scene.h"

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
    spawner = new ObjectSpawner(Scene::inst->rootNode);

	for (std::string id : LevelEventFactory::getEventIds())
	{
		levelEvents.emplace(id, new TypedLevelEvent(this, id));
	}

	for (int i = 0; i < 20; i++)
	{
		colorSlots.push_back(new ColorSlot());
	}

    std::ofstream oMain(levelDir / "level.aslv");
    oMain << toJsonMain();
    std::ofstream oMeta(levelDir / "meta.json");
    oMeta << toJsonMeta();
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

    json jMain, jMeta;

    {
        std::ifstream iMain(levelDir / "level.aslv");
        iMain >> jMain;

        std::ifstream iMeta(levelDir / "meta.json");
        iMeta >> jMeta;
    }

	name = jMeta["name"].get<std::string>();
	bpm = jMeta["bpm"].get<float>();
	offset = jMeta["offset"].get<float>();

	json::array_t objsArr = jMain["objects"];
	spawner = new ObjectSpawner(objsArr, Scene::inst->rootNode);

    // Load prefabs
    for (json& j : jMain["prefabs"])
    {
        Prefab* prefab = new Prefab(j);
        prefabs.emplace(prefab->id, prefab);
    }

	// Prepare level events map
	for (std::string& id : LevelEventFactory::getEventIds())
	{
		levelEvents[id] = nullptr;
	}

	json::array_t eventsArr = jMain["events"];
	for (json& eventJ : eventsArr)
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

	json::array_t colorSlotsArr = jMain["color_slots"];
	for (json& colorSlotJ : colorSlotsArr)
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

    for (ColorSlot* slot : colorSlots)
    {
        delete slot;
    }

    for (auto &[id, prefab] : prefabs)
    {
        delete prefab;
    }

	delete spawner;
	delete clip;
}

void Level::initEvents()
{
	onSelectObject += EventHandler<std::optional<std::reference_wrapper<LevelObject>>>(&AnimateableObjectBehaviour::selectObjectEventHandler);
	onSelectObject += EventHandler<std::optional<std::reference_wrapper<LevelObject>>>(&AnimateableColoredObjectBehaviour::selectObjectEventHandler);
	onSelectEvent += EventHandler<std::optional<std::reference_wrapper<TypedLevelEvent>>>(&AnimateableLevelEvent::selectLevelEventEventHandler);
	onSelectColorSlot += EventHandler<std::optional<std::reference_wrapper<ColorSlot>>>(&Themes::selectColorSlotEventHandler);
}

void Level::clearSelectedObject()
{
	selection.selectedObjects.clear();
	onSelectObject.invoke(std::nullopt);
}

void Level::clearSelectedEvent()
{
	selection.selectedEvent.reset();
	onSelectEvent.invoke(std::nullopt);
}

void Level::clearSelectedColorSlot()
{
	selection.selectedColorSlot.reset();
	onSelectColorSlot.invoke(std::nullopt);
}

void Level::setSelectedObject(LevelObject& levelObject)
{
    clearSelectedObject();
	selection.selectedObjects.emplace(&levelObject);
	onSelectObject.invoke(levelObject);
}

void Level::setSelectedEvent(TypedLevelEvent& levelEvent)
{
	selection.selectedEvent = levelEvent;
	onSelectEvent.invoke(levelEvent);
}

void Level::setSelectedColorSlot(ColorSlot& colorSlot)
{
    clearSelectedColorSlot();
	selection.selectedColorSlot = colorSlot;
	onSelectColorSlot.invoke(colorSlot);
}

void Level::addSelectedObject(LevelObject& levelObject)
{
    selection.selectedObjects.emplace(&levelObject);
}

void Level::removeSelectedObject(LevelObject& levelObject)
{
    selection.selectedObjects.erase(&levelObject);
}

bool Level::isObjectSelected(LevelObject &levelObject)
{
    return selection.selectedObjects.contains(&levelObject);
}

Selection Level::getSelection()
{
	return selection;
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

json Level::toJsonMain()
{
	json j;
	j["objects"] = spawner->toJson();
    json::array_t prefabsArr;
    for (auto &[id, prefab] : prefabs)
    {
        prefabsArr.push_back(prefab->toJson());
    }
    j["prefabs"] = prefabsArr;
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

json Level::toJsonMeta()
{
    json j;
    j["name"] = name;
    j["bpm"] = bpm;
    j["offset"] = offset;
    return j;
}

void Level::save()
{
	std::ofstream oMain(levelDir / "level.aslv");
    oMain << toJsonMain();
    std::ofstream oMeta(levelDir / "meta.json");
    oMeta << toJsonMeta();
}
