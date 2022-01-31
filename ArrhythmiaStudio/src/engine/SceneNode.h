#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "Transform.h"
#include "rendering/RendererComponent.h"

// Represents a node in a scene graph
class Scene;

class SceneNode
{
public:
	std::string name;

	// The node's transform
	Transform transform;

	SceneNode* parent = nullptr;
	std::vector<SceneNode*> children;

	std::unordered_set<SceneNode*> activeChildren;

	// The node's renderer
	RendererComponent* renderer = nullptr;

	SceneNode(std::string name);
	SceneNode(std::string name, SceneNode* parent);
    SceneNode(std::string name, bool isRoot);
	~SceneNode();

	void setParent(SceneNode* newParent);

	void setActive(bool value);
	bool getActive() const;
private:
	bool active;
};
