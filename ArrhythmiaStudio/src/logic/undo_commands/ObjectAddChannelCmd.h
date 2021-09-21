#pragma once

#include "../UndoCommand.h"
#include "../AnimationChannel.h"

class ObjectAddChannelCmd : public UndoCommand
{
public:
	ObjectAddChannelCmd(uint64_t objectId, const AnimationChannel* channel);
	~ObjectAddChannelCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	AnimationChannelType type;
	nlohmann::json data;
};