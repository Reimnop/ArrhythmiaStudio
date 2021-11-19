#pragma once
#include "SceneNode.h"

class LevelObject
{
public:
	std::string name;
	float startTime;
	float endTime;

	LevelObject(std::string name)
	{
		this->name = name;
	}

	virtual ~LevelObject() = 0 {};

	virtual SceneNode* getObjectRootNode() const = 0;

	virtual void update(float time) = 0;
	virtual void setObjectState(bool active) = 0;
};