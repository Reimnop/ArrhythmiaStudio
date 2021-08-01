#pragma once

#include <vector>
#include <random>
#include <functional>

#include "LevelObject.h"
#include "Sequencer.h"

class LevelManager {
public:
	std::vector<LevelObject> levelObjects;
	int selectedObjectIndex = -1;

	LevelManager();
private:
	Sequencer* sequencer;
};