#pragma once

#include "../UndoCommand.h"
#include "../AnimationChannel.h"

class ObjectAddKeyframeCmd : public UndoCommand
{
public:
	ObjectAddKeyframeCmd(uint64_t objectId, AnimationChannelType type, Keyframe kf);
	~ObjectAddKeyframeCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	AnimationChannelType type;
	nlohmann::json data;
};