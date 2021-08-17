#pragma once

#include <string>

class DataManager
{
public:
	static DataManager* inst;

	DataManager();

	void saveLevel(bool saveAs = false);
	void openLevel();
};
