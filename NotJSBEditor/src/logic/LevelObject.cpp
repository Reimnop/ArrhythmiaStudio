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