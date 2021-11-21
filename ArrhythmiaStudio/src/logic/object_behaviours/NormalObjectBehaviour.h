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

	void fromJson(json& j) override;
	void toJson(json& j) override;

	void drawEditor() override;
private:
	static ShapeManager* shapeManager;
};