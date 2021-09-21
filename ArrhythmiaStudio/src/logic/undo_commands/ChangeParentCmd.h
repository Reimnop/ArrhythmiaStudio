#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"

class ChangeParentCmd : public UndoCommand
{
public:
	ChangeParentCmd(const uint64_t objectId, const LevelObject* oldParent, const LevelObject* newParent);
	~ChangeParentCmd() override = default;

	void undo() override;
	void redo() override;
private:
	uint64_t id;
	uint64_t oldParentId;
	uint64_t newParentId;
};
