#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"

class ObjectAddKeyframeCmd : public UndoCommand
{
public:
	ObjectAddKeyframeCmd(const uint64_t objectId, const AnimationChannelType type, const Keyframe kf);
	~ObjectAddKeyframeCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	AnimationChannelType type;
	nlohmann::json data;
};