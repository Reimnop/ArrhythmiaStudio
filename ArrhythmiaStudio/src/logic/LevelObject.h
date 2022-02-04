#pragma once

#include <regex>

#include "json.hpp"
#include "ObjectSpawner.h"
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
	int row;
	int layer;
	SceneNode* node;
	LevelObjectBehaviour* behaviour;

	ObjectSpawner* spawner;

	LevelObject(std::string type, ObjectSpawner* spawner);
	LevelObject(json& j, ObjectSpawner* spawner);
	~LevelObject();

	// Only call after loading all objects
	void initializeParent();

	void update(float time);

	LevelObject* getParent();
	void setParent(LevelObject* newParent);

	void setName(std::string& name);
	std::string getName();

	void fromJson(json j);
	json toJson(float time = 0.0f);

	void drawEditor();
private:
	static inline std::string parentSearchStr;
	static inline std::regex parentSearchRegex = std::regex(parentSearchStr);
	static inline bool parentSearchOpen = false;

	std::string name;
	std::string type;

	// This feels like a very dirty hack. Too bad!
	uint64_t parentIdToInitialize = 0;

	LevelObject* parent = nullptr;
	std::unordered_set<LevelObject*> children;
};
