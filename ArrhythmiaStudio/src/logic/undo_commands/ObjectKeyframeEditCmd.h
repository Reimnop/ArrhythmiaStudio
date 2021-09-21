#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"

class ObjectKeyframeEditCmd : public UndoCommand
{
public:
	ObjectKeyframeEditCmd(const uint64_t objectId, const AnimationChannelType channelType, const Keyframe& oldKf, const Keyframe& newKf);
	~ObjectKeyframeEditCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	AnimationChannelType type;
	Keyframe oldKf;
	Keyframe newKf;
};