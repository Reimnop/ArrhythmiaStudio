#include "LevelObject.h"

LevelObject::LevelObject(std::string name) {
	this->name = name;
	startTime = 0.0f;
	killTime = 0.0f;
	editorBinIndex = 0;
	node = nullptr;
}

LevelObject::~LevelObject() {
	for (AnimationChannel* channel : animationChannels) {
		delete channel;
	}

	animationChannels.clear();
	animationChannels.shrink_to_fit();
}

void LevelObject::genActionPair(ObjectAction* spawnAction, ObjectAction* killAction) {
	ObjectAction spawn = ObjectAction();
	spawn.time = startTime;
	spawn.type = ObjectActionType_Spawn;
	spawn.levelObject = this;

	ObjectAction kill = ObjectAction();
	kill.time = killTime + 0.0001f;
	kill.type = ObjectActionType_Kill;
	kill.levelObject = this;

	*spawnAction = spawn;
	*killAction = kill;
}

void LevelObject::insertChannel(AnimationChannel* value) {
	if (animationChannels.size() == 0) {
		animationChannels.push_back(value);
		return;
	}

	std::vector<AnimationChannel*>::iterator it = std::lower_bound(animationChannels.begin(), animationChannels.end(), value,
		[](AnimationChannel* a, AnimationChannel* b) {
			return a->type < b->type;
		});
	animationChannels.insert(it, value);
}

bool LevelObject::hasChannel(AnimationChannelType channelType) {
	for (int i = 0; i < animationChannels.size(); i++) {
		if (animationChannels[i]->type == channelType) {
			return true;
		}
	}

	return false;
}