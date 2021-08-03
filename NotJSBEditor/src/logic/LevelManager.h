#pragma once

#include <vector>
#include <random>
#include <unordered_set>
#include <functional>

#include "../rendering/MeshRenderer.h"
#include "SceneNode.h"
#include "LevelObject.h"
#include "Timeline.h"
#include "DopeSheet.h"
#include "ObjectAction.h"

class LevelManager {
public:
	std::vector<LevelObject*> levelObjects;
	int selectedObjectIndex = -1;

	LevelManager();

	void update(float time);
	void updateAllObjectActions();
	void recalculateActionIndex(float time);
private:
	Timeline* timeline;
	DopeSheet* dopeSheet;

	float lastTime = 0.0f;

	int actionIndex = 0;

	std::vector<ObjectAction> objectActions;
	std::unordered_set<LevelObject*> aliveObjects;

	void insertAction(ObjectAction value);
	void spawnNode(LevelObject* levelObject);
};