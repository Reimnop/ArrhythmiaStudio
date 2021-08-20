#pragma once

#include <vector>
#include <unordered_set>

#include "AudioClip.h"
#include "LevelObject.h"
#include "Timeline.h"
#include "Properties.h"
#include "Theme.h"
#include "ObjectAction.h"
#include "ColorSlot.h"
#include "Level.h"

class Theme;

class LevelManager
{
public:
	static LevelManager* inst;

	AudioClip* audioClip;

	Level* level;

	int selectedObjectIndex = -1;

	float time = 0.0f;

	LevelManager();

	void loadLevel(std::string levelPath);

	void update();

	void updateLevel(float time);
	void updateObject(LevelObject* levelObject);
	void updateColorSlot(ColorSlot* colorSlot);

	void recalculateAllObjectActions();
	void recalculateObjectAction(LevelObject* levelObject);
	void recalculateActionIndex(float time);

	void insertObject(LevelObject* levelObject);
	void removeObject(LevelObject* levelObject);
private:
	Timeline* timeline;
	Properties* properties;
	Theme* theme;

	float lastTime = 0.0f;

	int actionIndex = 0;

	std::vector<ObjectAction> objectActions;
	std::unordered_set<LevelObject*> aliveObjects;

	void insertAction(ObjectAction value);
	void spawnObject(LevelObject* levelObject);
	void recursivelyInitializeObjectTree(nlohmann::json j, LevelObject* parent, Level* level);
};
