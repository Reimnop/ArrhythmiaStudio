#include <iostream>
#include "SceneNode.h"
#include "Scene.h"

SceneNode::SceneNode(std::string name)
{
	this->name = name;

	setActive(true);
	setParent(nullptr);
}

SceneNode::SceneNode(std::string name, SceneNode* parent)
{
	this->name = name;

	setParent(parent);
}

SceneNode::SceneNode(std::string name, bool isRoot)
{
    this->name = name;

    if (!isRoot)
    {
        setParent(nullptr);
    }
}

SceneNode::~SceneNode()
{
	delete renderer;

	// Remove from parent
	std::vector<SceneNode*>::iterator it = std::remove(parent->children.begin(), parent->children.end(), this);
	parent->children.erase(it);

	parent->activeChildren.erase(this);

	// Unparent children
	for (SceneNode* node : children)
	{
		node->setParent(nullptr);
	}
}

void SceneNode::setParent(SceneNode* newParent)
{
	if (parent)
	{
		std::vector<SceneNode*>::iterator it = std::remove(parent->children.begin(), parent->children.end(), this);
		parent->children.erase(it);
		parent->activeChildren.erase(this);
	}

	if (!newParent)
	{
		newParent = Scene::inst->rootNode;
	}

	if (newParent)
	{
		// Add to new parent
		newParent->children.push_back(this);

		if (active)
		{
			newParent->activeChildren.insert(this);
		}
	}

	parent = newParent;
}

void SceneNode::setActive(bool value)
{
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

bool SceneNode::getActive() const
{
	return active;
}
