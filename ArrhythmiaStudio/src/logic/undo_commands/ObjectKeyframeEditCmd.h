#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"

class ObjectKeyframeEditCmd : public UndoCommand
{
public:
	ObjectKeyframeEditCmd(uint64_t objectId, AnimationChannelType channelType, Keyframe oldKf, Keyframe newKf);
	~ObjectKeyframeEditCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	AnimationChannelType type;
	Keyframe oldKf;
	Keyframe newKf;
};