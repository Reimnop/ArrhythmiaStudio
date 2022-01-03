#pragma once

#include "AnimateableColoredObjectBehaviour.h"
#include "../../engine/rendering/MeshRenderer.h"
#include "../Shape.h"
#include "json.hpp"

using namespace nlohmann;

class NormalObjectBehaviour : public AnimateableColoredObjectBehaviour
{
public:
	NormalObjectBehaviour(LevelObject* baseObject);
	~NormalObjectBehaviour() override = default;

	static LevelObjectBehaviour* create(LevelObject* object);

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;

	void drawEditor() override;
protected:
	void drawSequences() override;
	void drawColorSequences() override;
private:
	MeshRenderer* renderer;

	Sequence opacity;
	ColorIndexSequence color;

	Shape shape;
	void setShape(std::string id);
};