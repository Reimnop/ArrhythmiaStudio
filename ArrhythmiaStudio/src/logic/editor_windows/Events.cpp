#include "Events.h"
#include "imgui/imgui.h"
#include "../GameManager.h"

std::string Events::getTitle()
{
	return "Events";
}

void Events::draw()
{
	GameManager* gameManager = GameManager::inst;
	Level* level = gameManager->level;

	if (ImGui::BeginChild("##events", ImVec2(0.0f, 120.0f), true))
	{
		// Deselect
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow) && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			level->selection.selectedEvent.reset();
		}

		for (TypedLevelEvent* levelEvent : gameManager->level->levelEvents)
		{
			if (ImGui::Selectable(
				levelEvent->getTitle().c_str(),
				level->selection.selectedEvent.has_value() ? 
				&level->selection.selectedEvent->get() == levelEvent : 
				false))
			{
				level->selection.selectedEvent = *levelEvent;
			}
		}
	}
	ImGui::EndChild();

	if (level->selection.selectedEvent.has_value())
	{
		level->selection.selectedEvent->get().drawEditor();
	}
}
