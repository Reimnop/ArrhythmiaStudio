#pragma once

#include "../ShapeManager.h"
#include "AnimateableObjectBehaviour.h"
#include "json.hpp"

using namespace nlohmann;

class NormalObjectBehaviour : public AnimateableObjectBehaviour 
{
public:
	NormalObjectBehaviour(LevelObject* baseObject);
	~NormalObjectBehaviour() override;

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;

	void drawEditor() override;
private:
	static ShapeManager* shapeManager;
};