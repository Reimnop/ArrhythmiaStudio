#pragma once

#include "LevelObjectBehaviour.h"
#include "json.hpp"

using namespace nlohmann;

class AnimateableObjectBehaviour : public LevelObjectBehaviour
{
public:
	AnimateableObjectBehaviour(LevelObject* baseObject);
	~AnimateableObjectBehaviour() override;

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;
	void drawEditor() override;
};