#pragma once

#include <vector>
#include <unordered_set>
#include <functional>

#include "../rendering/MeshRenderer.h"
#include "SceneNode.h"
#include "LevelObject.h"
#include "Timeline.h"
#include "Properties.h"
#include "ObjectAction.h"

class LevelManager
{
public:
	std::vector<LevelObject*> levelObjects;
	int selectedObjectIndex = -1;

	float time;

	LevelManager();

	void update(float time);
	void recalculateAllObjectActions();
	void recalculateObjectAction(LevelObject* levelObject);
	void recalculateActionIndex(float time);
private:
	Timeline* timeline;
	Properties* properties;

	float lastTime = 0.0f;

	int actionIndex = 0;

	std::vector<ObjectAction> objectActions;
	std::unordered_set<LevelObject*> aliveObjects;

	void insertAction(ObjectAction value);
	void spawnNode(LevelObject* levelObject);
};
