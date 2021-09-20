#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"

class ObjectAddChannelCmd : public UndoCommand
{
public:
	ObjectAddChannelCmd(const LevelObject* obj, const AnimationChannel* channel);
	~ObjectAddChannelCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	AnimationChannelType type;
	nlohmann::json data;
};