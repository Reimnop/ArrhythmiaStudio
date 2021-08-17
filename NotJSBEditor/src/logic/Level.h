#pragma once

#include <nlohmann/json.hpp>

#include "ColorSlot.h"
#include "LevelObject.h"

class Level
{
public:
	std::string name;
	std::string song;
	std::vector<ColorSlot*> colorSlots;
	std::vector<LevelObject*> levelObjects;

	Level();
	Level(nlohmann::json j);
	~Level();

	nlohmann::ordered_json toJson();
};
