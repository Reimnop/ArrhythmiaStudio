#pragma once

#include <vector>
#include <random>
#include <functional>

#include "LevelObject.h"
#include "Sequencer.h"

class LevelManager {
public:
	std::vector<LevelObject> levelObjects;

	LevelManager();
private:
	Sequencer* sequencer;
};