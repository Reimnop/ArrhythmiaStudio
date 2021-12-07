#pragma once
#include <string>

#include "json.hpp"
#include "LevelEvent.h"

using namespace nlohmann;

class TypedLevelEvent
{
public:
	TypedLevelEvent(Level* level, std::string type);
	~TypedLevelEvent();

	std::string getTitle();

	void update(float time);

	json toJson();
	void fromJson(json j);

	void drawEditor();
private:
	Level* level;
	LevelEvent* levelEvent;

	std::string type;
};
