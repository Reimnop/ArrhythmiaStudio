#pragma once

#include "SceneNode.h"

class Scene {
public:
	static Scene* inst;

	SceneNode* rootNode;

	Scene();
	~Scene();
};
