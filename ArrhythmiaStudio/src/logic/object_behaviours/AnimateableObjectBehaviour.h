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

	void fromJson(json& j) override;
	void toJson(json& j) override;
	void drawEditor() override;
};