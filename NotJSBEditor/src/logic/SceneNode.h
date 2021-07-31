#pragma once

#include <string>
#include <vector>
#include "Transform.h"
#include "../rendering/RendererComponent.h"

// Represents a node in a scene graph
class SceneNode {
public:
	std::string name;

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