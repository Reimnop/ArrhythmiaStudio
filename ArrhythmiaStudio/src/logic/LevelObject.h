#pragma once

#include "json.hpp"
#include "../engine/SceneNode.h"

using namespace nlohmann;

class LevelObjectBehaviour;
class Level;
class LevelObject
{
public:
	uint64_t id;
	float startTime;
	float endTime;
	int bin;
	Level* level;
	SceneNode* node;
	LevelObjectBehaviour* behaviour;

	LevelObject(std::string type, Level* level);
	LevelObject(json j);
	~LevelObject();

	void update(float time);

	void setName(std::string name);
	std::string getName();

	void fromJson(json j);
	json toJson();

	void drawEditor();
private:
	std::string name;
	std::string type;
};
