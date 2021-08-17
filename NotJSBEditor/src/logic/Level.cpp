#include "Level.h"

Level::Level() = default;

Level::Level(nlohmann::json j)
{
	name = j["name"].get<std::string>();
	song = j["song"].get<std::string>();

	nlohmann::json::array_t objects = j["objects"].get<nlohmann::json::array_t>();
	for (int i = 0; i < objects.size(); i++)
	{
		levelObjects.push_back(new LevelObject(objects[i]));
	}

	nlohmann::json::array_t slots = j["color_slots"].get<nlohmann::json::array_t>();
	for (int i = 0; i < slots.size(); i++)
	{
		colorSlots.push_back(new ColorSlot(slots[i]));
	}
}

Level::~Level()
{
	for (LevelObject* obj : levelObjects)
	{
		delete obj;
	}

	for (ColorSlot* slot : colorSlots)
	{
		delete slot;
	}

	levelObjects.clear();
	colorSlots.clear();
	levelObjects.shrink_to_fit();
	colorSlots.shrink_to_fit();
}

nlohmann::ordered_json Level::toJson()
{
	nlohmann::ordered_json j;
	j["name"] = name;
	j["song"] = song;
	j["objects"] = nlohmann::ordered_json::array();
	for (int i = 0; i < levelObjects.size(); i++)
	{
		j["objects"][i] = levelObjects[i]->toJson();
	}

	j["color_slots"] = nlohmann::ordered_json::array();
	for (int i = 0; i < colorSlots.size(); i++)
	{
		j["color_slots"][i] = colorSlots[i]->toJson();
	}

	return j;
}
