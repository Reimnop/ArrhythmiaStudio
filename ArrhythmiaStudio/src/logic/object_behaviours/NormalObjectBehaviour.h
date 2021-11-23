#pragma once

#include "AnimateableObjectBehaviour.h"
#include "json.hpp"

using namespace nlohmann;

class NormalObjectBehaviour : public AnimateableObjectBehaviour 
{
public:
	NormalObjectBehaviour(LevelObject* baseObject);
	~NormalObjectBehaviour() override;

	static LevelObjectBehaviour* create(LevelObject* object);

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;

	void drawEditor() override;
};