#pragma once

#include <optional>
#include <vector>

#include "../animation/ColorSequence.h"
#include "EditorWindow.h"

#include "../data/ColorKeyframeInfo.h"
#include "../data/ColorKeyframeTimeEditInfo.h"

class Themes : public EditorWindow
{
public:
	~Themes() override = default;

	std::string getTitle() override;
	void draw() override;
private:
	static inline std::vector<std::tuple<std::reference_wrapper<ColorSequence>, std::string>> sequencesToDraw;
	static inline std::optional<ColorKeyframeInfo> selectedKeyframe;
	static inline std::optional<ColorKeyframeTimeEditInfo> timeEditingKeyframe;

	void drawEditor();
	bool beginKeyframeEditor();
	void sequenceEdit(ColorSequence& sequence, std::string label);
	void endKeyframeEditor();

	void beginKeyframeTimeEdit(int index, ColorSequence& sequence);
	void endKeyframeTimeEdit();
	void updateKeyframeTimeEdit();
	bool isKeyframeTimeEditing();
};
