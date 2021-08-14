#pragma once

#include <vector>
#include <string>

#include "animation/AnimationChannel.h"
#include "ObjectAction.h"
#include "SceneNode.h"
#include "ColorSlot.h"

class LevelObject
{
public:
	std::string name;
	float startTime = 0.0f;
	float killTime = 5.0f;
	int editorBinIndex = 0;
	int colorSlotIndex = 0;

	float depth = 0.0f;

	std::vector<AnimationChannel*> animationChannels;

	SceneNode* node;

	LevelObject(std::string name);
	~LevelObject();

	void genActionPair(ObjectAction* spawnAction, ObjectAction* killAction);
	void insertChannel(AnimationChannel* value);
	bool hasChannel(AnimationChannelType channelType);
};
