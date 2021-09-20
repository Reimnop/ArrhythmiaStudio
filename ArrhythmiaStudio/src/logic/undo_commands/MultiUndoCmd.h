#pragma once

#include "../UndoCommand.h"
#include <vector>

template <typename T>
class MultiUndoCmd : public UndoCommand
{
	static_assert(std::is_pointer_v<T>, "Expected a pointer");
public:
	MultiUndoCmd(const std::vector<T>& cmds)
	{
		commands = std::vector<T>(cmds);
	}

	~MultiUndoCmd() override
	{
		for (const T cmd : commands)
		{
			delete cmd;
		}
	}

	void undo() override
	{
		for (T cmd : commands)
		{
			cmd->undo();
		}
	}

	void redo() override
	{
		for (T cmd : commands)
		{
			cmd->redo();
		}
	}
private:
	std::vector<T> commands;
};
