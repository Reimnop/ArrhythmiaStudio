#include "LevelManager.h"

#include <string>

LevelManager::LevelManager() {
	std::srand(8);

	// Add random objects
    for (int i = 0; i < 4; i++)
    {
        float start = std::rand() / (float)RAND_MAX * 40.0f;
        float end = start + 20.0f + std::rand() / (float)RAND_MAX * 20.0f;

        LevelObject obj;
        obj.name = "Object Index " + std::to_string(i);
        obj.startTime = start;
        obj.endTime = end;
        obj.editorBinIndex = (int)(std::rand() / (float)RAND_MAX * 10.0f);

        levelObjects.push_back(obj);
    }

    sequencer = new Sequencer(this);
}