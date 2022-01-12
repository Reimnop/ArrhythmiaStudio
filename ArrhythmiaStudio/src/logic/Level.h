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
	static inline Level* inst;

	path levelDir;
	std::string name;
	AudioClip* clip;
	
	std::unordered_map<std::string, TypedLevelEvent*> levelEvents;
	std::vector<ColorSlot*> colorSlots;
	ObjectSpawner* spawner;

	Selection selection;

	float time = 0.0f;
	float levelLength;

	float offset = 0.0f;
	float bpm = 120.0f;

	// This constructor creates a new level
	Level(std::string name, path songPath, path levelDir);
	// This constructor opens an existing level
	Level(path levelDir);
	~Level();

	void seek(float t);
	void update();

	json toJson();
	void save();
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Level");
};