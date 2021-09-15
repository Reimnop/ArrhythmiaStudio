#pragma once

#include <nlohmann/json.hpp>
#include "UndoActionType.h"

struct UndoAction
{
	UndoActionType type;
	nlohmann::json data;
};
