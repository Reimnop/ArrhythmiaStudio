#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include "AnimationChannel.h"
#include "ObjectAction.h"
#include "SceneNode.h"
#include "LevelObjectProperties.h"

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
	int layer = 0;

	// Editor data do not touch
	bool timelineHighlighted = false;
	// [----------------------]

	uint64_t id;
	uint64_t parentId;

	// DO NOT MODIFY!
	std::unordered_set<uint64_t> childrenId;

	std::vector<AnimationChannel*> animationChannels;

	SceneNode* node;

	LevelObject();
	LevelObject(nlohmann::json j);
	~LevelObject();

	LevelObjectProperties dumpProperties() const;
	void applyProperties(LevelObjectProperties properties);

	void setParent(LevelObject* newParent);

	void genActionPair(ObjectAction* spawnAction, ObjectAction* killAction);
	void insertChannel(AnimationChannel* value);
	void eraseChannel(AnimationChannelType type);
	AnimationChannel* getChannel(AnimationChannelType type);
	bool hasChannel(AnimationChannelType type);
	nlohmann::ordered_json toJson();
private:
	bool animationChannelLookup[AnimationChannelType_Count];
};
