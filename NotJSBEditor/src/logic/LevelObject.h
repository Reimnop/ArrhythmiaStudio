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
	float startTime;
	float killTime;
	int editorBinIndex;

	ColorSlot* colorSlot;
	std::vector<AnimationChannel*> animationChannels;

	SceneNode* node;

	LevelObject(std::string name);
	~LevelObject();

	void genActionPair(ObjectAction* spawnAction, ObjectAction* killAction);
	void insertChannel(AnimationChannel* value);
	bool hasChannel(AnimationChannelType channelType);
};
