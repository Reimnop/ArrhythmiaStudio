#pragma once

#include "AnimateableObjectBehaviour.h"
#include "../../engine/rendering/TextRenderer.h"
#include "json.hpp"

using namespace nlohmann;

class TextObjectBehaviour : public AnimateableObjectBehaviour 
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
private:
	static inline Font* font;

	TextRenderer* renderer;
	std::wstring text;

	Sequence opacity;
};