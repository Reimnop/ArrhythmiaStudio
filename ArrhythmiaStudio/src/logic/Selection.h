#pragma once
#include "LevelObject.h"
#include "level_events/TypedLevelEvent.h"
#include "ColorSlot.h"

struct Selection
{
	std::optional<std::reference_wrapper<LevelObject>> selectedObject;
	std::optional<std::reference_wrapper<TypedLevelEvent>> selectedEvent;
	std::optional<std::reference_wrapper<ColorSlot>> selectedColorSlot;
};
