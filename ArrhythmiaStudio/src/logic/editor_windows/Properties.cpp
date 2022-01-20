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
	if (!selection.selectedObjects.empty())
	{
		if (selection.selectedObjects.size() == 1)
        {
            LevelObject* obj = *selection.selectedObjects.begin();
            obj->drawEditor();
        }
        else
        {
            ImGui::Text("Multiple objects selected");
        }
	}
	else
	{
		ImGui::Text("No object selected");
	}
}