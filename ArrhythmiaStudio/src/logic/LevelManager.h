#pragma once

#include <vector>
#include <unordered_set>

#include "AudioClip.h"
#include "LevelObject.h"
#include "Timeline.h"
#include "Properties.h"
#include "Theme.h"
#include "Events.h"
#include "ObjectAction.h"
#include "ColorSlot.h"
#include "Level.h"
#include "../rendering/Shader.h"
#include "../rendering/TextRenderer.h"

class LevelManager
{
public:
	static LevelManager* inst;

	Font* mainFont;
	Shader* unlitShader;

	AudioClip* audioClip;

	Level* level;

	std::unordered_set<LevelObject*> selectedObjects;

	float time = 0.0f;

	LevelManager();

	void loadLevel(std::string levelPath);

	void update();

	void updateLevel(float time);
	void updateColorSlot(ColorSlot* colorSlot);
	void updateLevelEvent(LevelEvent* levelEvent);
	void updateObject(LevelObject* levelObject);

	void recalculateAllObjectActions();
	void recalculateObjectAction(LevelObject* levelObject);
	void recalculateActionIndex(float time);

	void insertObject(LevelObject* levelObject);
	void removeObject(LevelObject* levelObject);

	// Returns false if initialization failed
	bool initializeObjectParent(LevelObject* levelObject);
private:
	Timeline* timeline;
	Properties* properties;
	Theme* theme;
	Events* events;

	float lastTime = 0.0f;

	int actionIndex = 0;

	std::vector<ObjectAction> objectActions;
	std::unordered_set<LevelObject*> aliveObjects;

	bool indexAdvance(float time) const;
	bool indexReverse(float time) const;

	void insertAction(ObjectAction value);
	void spawnObject(LevelObject* levelObject) const;
};
