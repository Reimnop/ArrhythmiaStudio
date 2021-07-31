#include "SceneNode.h"

SceneNode::SceneNode(std::string name) {
	this->name = name;
	transform = new Transform();

	parent = nullptr;
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
}

void SceneNode::setParent(SceneNode* newParent) {
	if (parent) {
		// Find index in parent's children vector
		int index;
		for (int i = 0; i < parent->children.size(); i++) {
			if (parent->children[i] == this) {
				index = i;
				break;
			}
		}

		// Remove from parent
		parent->children.erase(parent->children.begin() + index);
	}

	if (newParent) {
		parent = newParent;

		// Add to new parent
		newParent->children.push_back(this);
	}
}