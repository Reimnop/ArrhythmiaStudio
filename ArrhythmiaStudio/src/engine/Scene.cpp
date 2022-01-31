#include "Scene.h"

Scene* Scene::inst;

Scene::Scene()
{
	if (inst)
	{
		return;
	}

	inst = this;

	rootNode = new SceneNode("Root Node", true);
}

Scene::~Scene()
{
	delete rootNode;
}
