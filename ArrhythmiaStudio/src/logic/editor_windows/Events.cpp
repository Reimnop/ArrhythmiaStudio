#include "Events.h"
#include "imgui/imgui.h"
#include "../GameManager.h"

std::string Events::getTitle()
{
	return "Events";
}

void Events::draw()
{
	GameManager& gameManager = *GameManager::inst;
	std::unique_ptr<Level>& level = gameManager.level;

	Selection selection = level->getSelection();
	if (ImGui::BeginChild("##events", ImVec2(0.0f, 120.0f), true))
	{
		// Deselect
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow) && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			level->clearSelectedEvent();
		}

		for (auto &[type, levelEvent] : gameManager.level->levelEvents)
		{
			if (ImGui::Selectable(
				levelEvent->getTitle().c_str(),
				selection.selectedEvent.has_value() ?
				&selection.selectedEvent->get() == levelEvent :
				false))
			{
				level->setSelectedEvent(*levelEvent);
			}
		}
	}
	ImGui::EndChild();

	if (selection.selectedEvent.has_value())
	{
		selection.selectedEvent->get().drawEditor();
	}
}
