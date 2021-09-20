#pragma once

#include "../UndoCommand.h"
#include "../LevelObject.h"

class AddObjectCmd : public UndoCommand
{
public:
	AddObjectCmd(LevelObject* obj);
	~AddObjectCmd() override = default;

	void undo() override;
	void redo() override;
private:
	nlohmann::json data;
	std::vector<uint64_t> children;
};
