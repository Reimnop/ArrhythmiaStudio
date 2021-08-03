#include "Scene.h"

Scene* Scene::inst;

Scene::Scene() {
	if (inst) {
		return;
	}

	inst = this;

	rootNode = new SceneNode("Root Node");
}

Scene::~Scene() {
	delete rootNode;
}