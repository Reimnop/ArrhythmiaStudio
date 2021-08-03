#include "SceneNode.h"
#include "Scene.h"

SceneNode::SceneNode(std::string name) {
	this->name = name;
	transform = new Transform();

	setActive(true);
	setParent(nullptr);
}

SceneNode::SceneNode(std::string name, SceneNode* parent) {
	this->name = name;
	transform = new Transform();

	setParent(parent);
}

SceneNode::~SceneNode() {
	if (renderer) {
		delete renderer;
	}

	setParent(nullptr);
	delete transform;

	// Delete children
	for (SceneNode* node : children) {
		delete node;
	}

	children.clear();
	children.shrink_to_fit();
}

void SceneNode::setParent(SceneNode* newParent) {
	if (parent) {
		std::vector<SceneNode*>::iterator it = std::find(parent->children.begin(), parent->children.end(), this);
		parent->children.erase(it);
	}

	if (!newParent) {
		newParent = Scene::inst->rootNode;
	}

	if (newParent) {
		parent = newParent;

		// Add to new parent
		newParent->children.push_back(this);
	}
}

void SceneNode::setActive(bool value) {
	if (parent && active != value) 
	{
		if (value) 
		{
			parent->activeChildren.insert(this);
		}
		else 
		{
			parent->activeChildren.erase(this);
		}
	}

	active = value;
}

bool SceneNode::getActive() {
	return active;
}