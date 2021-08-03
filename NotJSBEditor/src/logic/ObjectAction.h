#pragma once

#include "ObjectActionType.h"

struct ObjectAction {
	float time;
	ObjectActionType type;
	LevelObject* levelObject;
};