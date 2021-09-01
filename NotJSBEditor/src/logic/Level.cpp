#include "Level.h"

Level::~Level()
{
	for (const ColorSlot* colorSlot : colorSlots)
	{
		delete colorSlot;
	}

	for (const LevelEvent* levelEvent : levelEvents)
	{
		delete levelEvent;
	}

	for (const LevelObject* levelObject : levelObjects)
	{
		delete levelObject;
	}
}

void Level::insertLevelEvent(LevelEvent* value)
{
	if (hasLevelEvent(value->type))
	{
		return;
	}

	std::vector<LevelEvent*>::iterator it = std::lower_bound(levelEvents.begin(), levelEvents.end(),
	                                                         value,
	                                                         [](const LevelEvent* a, const LevelEvent* b)
	                                                         {
		                                                         return a->type < b->type;
	                                                         });
	levelEvents.insert(it, value);

	levelEventLookup[value->type] = true;
}

bool Level::hasLevelEvent(LevelEventType type)
{
	return levelEventLookup[type];
}

nlohmann::ordered_json Level::toJson()
{
	nlohmann::ordered_json j;
	j["name"] = name;

	j["color_slots"] = nlohmann::ordered_json::array();
	for (int i = 0; i < colorSlots.size(); i++)
	{
		j["color_slots"][i] = colorSlots[i]->toJson();
	}

	j["level_events"] = nlohmann::ordered_json::array();
	for (int i = 0; i < levelEvents.size(); i++)
	{
		j["level_events"][i] = levelEvents[i]->toJson();
	}

	j["objects"] = nlohmann::ordered_json::array();
	for (int i = 0; i < levelObjects.size(); i++)
	{
		if (levelObjects[i]->parent == nullptr)
		{
			j["objects"].push_back(levelObjects[i]->toJson());
		}
	}

	return j;
}
