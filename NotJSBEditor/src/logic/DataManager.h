#pragma once

#include <string>

#include "LevelCreateInfo.h"

class DataManager
{
public:
	static DataManager* inst;

	std::string levelDir;

	DataManager();

	void newLevel(LevelCreateInfo createInfo);
	void saveLevel(bool saveAs = false);
	void openLevel();
private:
	void genLevelFiles(LevelCreateInfo createInfo);
};
