#pragma once

#include "LevelObjectBehaviour.h"
#include "json.hpp"
#include "KeyframeInfo.h"
#include "KeyframeTimeEditInfo.h"
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
	static std::vector<std::reference_wrapper<Sequence>> sequencesToDraw;
	static std::optional<KeyframeInfo> selectedKeyframe;
	static std::optional<KeyframeTimeEditInfo> timeEditingKeyframe;

	Sequence positionX;
	Sequence positionY;
	Sequence scaleX;
	Sequence scaleY;
	Sequence rotation;

	virtual void drawSequences();
protected:
	bool beginKeyframeEditor();
	void sequenceEdit(Sequence& sequence);
	void endKeyframeEditor();

	void beginKeyframeTimeEdit(int index, Sequence& sequence);
	void endKeyframeTimeEdit();
	void updateKeyframeTimeEdit();
	bool isKeyframeTimeEditing();
};