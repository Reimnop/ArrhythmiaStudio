#pragma once

#include "ObjectActionType.h"

class LevelObject;

struct ObjectAction
{
	float time;
	ObjectActionType type;
	LevelObject* levelObject;
};
