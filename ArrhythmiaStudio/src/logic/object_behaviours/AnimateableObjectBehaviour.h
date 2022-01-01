#pragma once

#include "LevelObjectBehaviour.h"
#include "json.hpp"
#include "../data/KeyframeInfo.h"
#include "../data/KeyframeTimeEditInfo.h"
#include "../animation/Sequence.h"

using namespace nlohmann;

class AnimateableObjectBehaviour : public LevelObjectBehaviour
{
public:
	AnimateableObjectBehaviour(LevelObject* baseObject);
	~AnimateableObjectBehaviour() override = default;

	void update(float time) override;

	void readJson(json& j) override;
	void writeJson(json& j) override;
	void drawEditor() override;
private:
	static inline std::vector<std::tuple<std::reference_wrapper<Sequence>, std::string>> sequencesToDraw;
	static inline std::optional<KeyframeInfo> selectedKeyframe;
	static inline std::optional<KeyframeTimeEditInfo> timeEditingKeyframe;

	Sequence positionX;
	Sequence positionY;
	Sequence positionZ;
	Sequence scaleX;
	Sequence scaleY;
	Sequence rotation;
protected:
	virtual void drawSequences();

	bool beginKeyframeEditor();
	void sequenceEdit(Sequence& sequence, std::string label);
	void endKeyframeEditor();

	void beginKeyframeTimeEdit(int index, Sequence& sequence);
	void endKeyframeTimeEdit();
	void updateKeyframeTimeEdit();
	bool isKeyframeTimeEditing();
};