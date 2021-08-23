#pragma once

#include <nlohmann/json.hpp>

#include "ColorSlot.h"
#include "LevelEvent.h"
#include "LevelObject.h"

class Level
{
public:
	std::string name;
	std::vector<ColorSlot*> colorSlots;
	std::vector<LevelEvent*> levelEvents;
	std::vector<LevelObject*> levelObjects;

	~Level();

	void insertLevelEvent(LevelEvent* value);
	bool hasLevelEvent(LevelEventType type);
	nlohmann::ordered_json toJson();
private:
	bool levelEventLookup[LevelEventType_Count];
};
