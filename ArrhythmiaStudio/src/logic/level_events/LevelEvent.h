#pragma once
#include <string>

#include "json.hpp"
using namespace nlohmann;

class Level;
class LevelEvent
{
public:
	LevelEvent(Level* level) : level(level) {}
	virtual ~LevelEvent() = default;

	virtual std::string getTitle() = 0;

	virtual void update(float time) = 0;
	virtual void drawEditor() = 0;

	virtual void toJson(json& j) = 0;
	virtual void fromJson(json& j) = 0;
protected:
	Level* level;
};