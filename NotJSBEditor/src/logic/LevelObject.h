#pragma once

#include <vector>
#include <string>

#include "animation/AnimationChannel.h"
#include "SceneNode.h"

class LevelObject {
public:
	std::string name;
	float startTime;
	float killTime;
	int editorBinIndex;

	std::vector<AnimationChannel*> animationChannels;

	SceneNode* node;

	LevelObject(std::string name);
	~LevelObject();
};