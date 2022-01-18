#pragma once

#include "AnimateableObjectBehaviour.h"
#include "json.hpp"
#include "../data/ColorIndexKeyframeInfo.h"
#include "../data/ColorIndexKeyframeTimeEditInfo.h"
#include "../animation/ColorIndexSequence.h"

using namespace nlohmann;

class AnimateableColoredObjectBehaviour : public AnimateableObjectBehaviour
{
public:
	AnimateableColoredObjectBehaviour(LevelObject* baseObject) : AnimateableObjectBehaviour(baseObject)
	{
	}
	~AnimateableColoredObjectBehaviour() override = default;

	void drawEditor() override;

	static void selectObjectEventHandler(std::optional<std::reference_wrapper<LevelObject>> object);
private:
	static inline std::vector<std::tuple<std::reference_wrapper<ColorIndexSequence>, std::string>> sequencesToDraw;
	static inline std::optional<ColorIndexKeyframeInfo> selectedKeyframe;
	static inline std::optional<ColorIndexKeyframeTimeEditInfo> timeEditingKeyframe;
protected:
	virtual void drawColorSequences() = 0;

	bool beginColorKeyframeEditor();
	void colorSequenceEdit(ColorIndexSequence& sequence, std::string label);
	void endColorKeyframeEditor();

	void beginColorKeyframeTimeEdit(int index, ColorIndexSequence& sequence);
	void endColorKeyframeTimeEdit();
	void updateColorKeyframeTimeEdit();
	bool isColorKeyframeTimeEditing();
};