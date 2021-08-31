#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include "AnimationChannel.h"
#include "ObjectAction.h"
#include "SceneNode.h"

class LevelObject
{
public:
	std::string name = "Untitled object";
	float startTime = 0.0f;
	float killTime = 5.0f;
	int shapeIndex = 0;
	int editorBinIndex = 0;
	int colorSlotIndex = 0;
	float depth = 0.0f;

	// Editor data do not touch
	bool timelineHighlighted = false;
	// [----------------------]

	LevelObject* parent;
	std::vector<LevelObject*> children;

	std::vector<AnimationChannel*> animationChannels;

	SceneNode* node;

	LevelObject();
	LevelObject(nlohmann::json j);
	~LevelObject();

	void setParent(LevelObject* newParent);

	void genActionPair(ObjectAction* spawnAction, ObjectAction* killAction);
	void insertChannel(AnimationChannel* value);
	bool hasChannel(AnimationChannelType channelType);
	nlohmann::ordered_json toJson(bool excludeChildren = false);
private:
	bool animationChannelLookup[AnimationChannelType_Count];
};
