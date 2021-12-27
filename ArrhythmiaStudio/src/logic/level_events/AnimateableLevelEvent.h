#pragma once
#include <optional>
#include <vector>

#include "LevelEvent.h"
#include "../animation/Sequence.h"
#include "../data/KeyframeInfo.h"
#include "../data/KeyframeTimeEditInfo.h"

class AnimateableLevelEvent : public LevelEvent
{
public:
	AnimateableLevelEvent(Level* level) : LevelEvent(level) {}

	void drawEditor() override;
private:
	static inline std::vector<std::tuple<std::reference_wrapper<Sequence>, std::string>> sequencesToDraw;
	static inline std::optional<KeyframeInfo> selectedKeyframe;
	static inline std::optional<KeyframeTimeEditInfo> timeEditingKeyframe;
protected:
	virtual void drawSequences() = 0;

	bool beginKeyframeEditor();
	void sequenceEdit(Sequence& sequence, std::string label);
	void endKeyframeEditor();

	void beginKeyframeTimeEdit(int index, Sequence& sequence);
	void endKeyframeTimeEdit();
	void updateKeyframeTimeEdit();
	bool isKeyframeTimeEditing();
};
