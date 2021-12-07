#pragma once

#include <unordered_map>
#include <unordered_set>
#include "Selection.h"

class LevelObject;
class Level 
{
public:
	std::unordered_map<uint64_t, LevelObject*> levelObjects;
	std::vector<TypedLevelEvent*> levelEvents;
	Selection selection;

	float time = 0.0f;
	float levelLength;

	Level();
	~Level();

	void seek(float t);
	void update();

	void addObject(LevelObject* object);
	void deleteObject(LevelObject* object);

	void insertObject(LevelObject* object);
	void removeObject(LevelObject* object);

	void insertActivateList(LevelObject* object);
	void insertDeactivateList(LevelObject* object);
	void removeActivateList(LevelObject* object);
	void removeDeactivateList(LevelObject* object);

	void recalculateObjectsState();
private:
	float lastTime = 0.0f;
	
	std::unordered_set<LevelObject*> aliveObjects;

	int activateIndex = 0;
	int deactivateIndex = 0;
	std::vector<LevelObject*> activateList;
	std::vector<LevelObject*> deactivateList;

	void updateForward();
	void updateReverse();
};