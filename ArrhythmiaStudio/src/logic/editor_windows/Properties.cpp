#include "Properties.h"
#include "imgui/imgui.h"
#include "../GameManager.h"

std::string Properties::getTitle()
{
	return "Properties";
}

void Properties::draw()
{
	Level& level = *GameManager::inst->level;

	if (level.selection.selectedObject.has_value())
	{
		level.selection.selectedObject.value().get().drawEditor();
	}
	else
	{
		ImGui::Text("No object selected");
	}
}