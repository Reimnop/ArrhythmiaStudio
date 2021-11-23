#pragma once
#include "LevelObject.h"

struct Selection
{
	std::optional<std::reference_wrapper<LevelObject>> selectedObject;
};
