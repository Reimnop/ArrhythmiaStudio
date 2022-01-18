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

	Selection selection = level.getSelection();
	if (selection.selectedObject.has_value())
	{
		selection.selectedObject.value().get().drawEditor();
	}
	else
	{
		ImGui::Text("No object selected");
	}
}