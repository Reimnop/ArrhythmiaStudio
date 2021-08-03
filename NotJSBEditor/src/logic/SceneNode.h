#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "Transform.h"
#include "../rendering/RendererComponent.h"

// Represents a node in a scene graph
class Scene;
class SceneNode {
public:
	std::string name;

	// Active state for the node. Does not affect children.
	bool active;

	// The node's transform
	Transform* transform;

	SceneNode* parent;
	std::vector<SceneNode*> children;

	// The node's renderer
	RendererComponent* renderer;

	SceneNode(std::string name);
	SceneNode(std::string name, SceneNode* parent);
	~SceneNode();

	void setParent(SceneNode* newParent);
};