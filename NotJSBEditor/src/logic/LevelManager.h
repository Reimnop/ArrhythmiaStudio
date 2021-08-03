#pragma once

#include <vector>
#include <random>
#include <tuple>
#include <unordered_set>
#include <functional>

#include "../rendering/MeshRenderer.h"
#include "SceneNode.h"
#include "LevelObject.h"
#include "Sequencer.h"
#include "ObjectAction.h"

typedef std::tuple<SceneNode, LevelObject> SceneNodeLevelObjectPair;

class LevelManager {
public:
	std::vector<LevelObject*> levelObjects;

	LevelManager();

	void update(float time);
	void updateAllObjectActions();
	void recalculateActionIndex(float time);
private:
	Sequencer* sequencer;

	float lastTime = 0.0f;

	int actionIndex = 0;

	std::vector<ObjectAction> objectActions;
	std::unordered_set<LevelObject*> aliveObjects;

	void insertAction(ObjectAction value);
	void spawnNode(LevelObject* levelObject);
};