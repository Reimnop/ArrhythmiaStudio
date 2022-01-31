#pragma once

#include <unordered_set>
#include <unordered_map>

#include "json.hpp"
#include "../engine/SceneNode.h"

using namespace nlohmann;

class LevelObject;
class ObjectSpawner
{
public:
	ObjectSpawner(SceneNode* parent) : parent(parent) { };
	ObjectSpawner(json::array_t& j, SceneNode* parent);
	~ObjectSpawner();

	std::unordered_map<uint64_t, LevelObject*> levelObjects;

	void update(float time);

	void addObject(LevelObject* object);
	void deleteObject(LevelObject* object);

	void insertObject(LevelObject* object);
	void removeObject(LevelObject* object);

	void insertActivateList(LevelObject* object);
	void insertDeactivateList(LevelObject* object);
	void removeActivateList(LevelObject* object);
	void removeDeactivateList(LevelObject* object);

	void recalculateObjectsState();

    SceneNode* getParent();

	json::array_t toJson();
private:
    SceneNode* parent;

	float currentTime = 0.0f;
	float lastTime = 0.0f;

	std::unordered_set<LevelObject*> aliveObjects;

	int activateIndex = 0;
	int deactivateIndex = 0;
	std::vector<LevelObject*> activateList;
	std::vector<LevelObject*> deactivateList;

	void updateForward();
	void updateReverse();
};