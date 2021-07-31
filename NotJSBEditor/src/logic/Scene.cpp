#include "Scene.h"

Scene::Scene() {
	rootNode = new SceneNode("Root Node");
}

Scene::~Scene() {
	delete rootNode;
}