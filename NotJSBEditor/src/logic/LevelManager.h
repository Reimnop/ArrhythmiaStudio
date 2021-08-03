#pragma once

#include <vector>
#include <random>
#include <tuple>
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
private:
	Sequencer* sequencer;

	float lastTime = 0.0f;

	int actionIndex = 0;

	std::vector<ObjectAction> objectActions;

	void insertAction(ObjectAction value);
	void spawnNode(LevelObject* levelObject);
};