#pragma once

#include "AnimateableColoredObjectBehaviour.h"
#include "../../engine/rendering/TextRenderer.h"
#include "json.hpp"

using namespace nlohmann;

class TextObjectBehaviour : public AnimateableColoredObjectBehaviour 
{
public:
	TextObjectBehaviour(LevelObject* baseObject);
	~TextObjectBehaviour() override = default;

	static LevelObjectBehaviour* create(LevelObject* object);

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;

	void drawEditor() override;
protected:
	void drawSequences() override;
	void drawColorSequences() override;
private:
	static inline Font* font;

	TextRenderer* renderer;
	std::string text;

	Sequence opacity;
	ColorIndexSequence color;
};