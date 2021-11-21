#pragma once

#include "json.hpp"
#include "SceneNode.h"

using namespace nlohmann;

class LevelObjectBehaviour;
class LevelObject
{
public:
	uint64_t id;
	float startTime;
	float endTime;
	SceneNode* node;
	LevelObjectBehaviour* behaviour;

	LevelObject();
	~LevelObject();

	template<typename T>
	void initializeObjectBehaviour()
	{
		behaviour = new T(this);
	}

	void setName(std::string name);
	std::string getName();

	void fromJson(json j);
	json toJson();

	void drawEditor();
private:
	std::string name;
};
