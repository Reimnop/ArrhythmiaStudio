#pragma once

#include <memory>

#include "../UndoCommand.h"
#include "../LevelObject.h"
#include "ObjectAddKeyframeCmd.h"

class ObjectRemoveKeyframeCmd : public UndoCommand
{
public:
	ObjectRemoveKeyframeCmd(const LevelObject* obj, const AnimationChannelType type, const Keyframe kf);
	~ObjectRemoveKeyframeCmd() override = default;

	void undo() override;
	void redo() override;
private:
	std::unique_ptr<ObjectAddKeyframeCmd> addKfCmd;
};