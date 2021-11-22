#pragma once
#include "json.hpp"
#include "../LevelObject.h"

using namespace nlohmann;

class LevelObjectBehaviour
{
public:
	LevelObjectBehaviour(LevelObject* baseObject) : baseObject(baseObject) {}
	virtual ~LevelObjectBehaviour() = default;

	virtual void update(float time) = 0;

	virtual void readJson(json& j) = 0;
	virtual void writeJson(json& j) = 0;
	virtual void drawEditor() = 0;
protected:
	LevelObject* baseObject;
};