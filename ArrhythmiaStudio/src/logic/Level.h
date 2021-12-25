#pragma once

#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include "log4cxx/logger.h"
#include "../engine/AudioClip.h"
#include "ColorSlot.h"
#include "Selection.h"

using namespace std::filesystem;

class LevelObject;
class Level 
{
public:
	path levelDir;
	std::string name;
	AudioClip* clip;

	std::unordered_map<uint64_t, LevelObject*> levelObjects;
	std::unordered_map<std::string, TypedLevelEvent*> levelEvents;
	std::vector<ColorSlot*> colorSlots;
	Selection selection;

	float time = 0.0f;
	float levelLength;

	float offset = 0.0f;
	float bpm = 120.0f;

	// This constructor creates a new level
	Level(std::string name, path audioPath, path levelDir);
	// This constructor opens an existing level
	Level(path levelDir);
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

	json toJson();
	void save();
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Level");

	float lastTime = 0.0f;
	
	std::unordered_set<LevelObject*> aliveObjects;

	int activateIndex = 0;
	int deactivateIndex = 0;
	std::vector<LevelObject*> activateList;
	std::vector<LevelObject*> deactivateList;

	void updateForward();
	void updateReverse();
};