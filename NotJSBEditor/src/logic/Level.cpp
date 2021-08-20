#include "Level.h"

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

	j["objects"] = nlohmann::ordered_json::array();
	for (int i = 0; i < levelObjects.size(); i++)
	{
		if (levelObjects[i]->parent == nullptr) 
		{
			j["objects"].push_back(levelObjects[i]->toJson());
		}
	}

	j["color_slots"] = nlohmann::ordered_json::array();
	for (int i = 0; i < colorSlots.size(); i++)
	{
		j["color_slots"][i] = colorSlots[i]->toJson();
	}

	return j;
}
