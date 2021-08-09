#include "Theme.h"

Theme::Theme()
{
	ImGuiController::onLayout.push_back(std::bind(&Theme::onLayout, this));
}

void Theme::onLayout()
{
	LevelManager* levelManager = LevelManager::inst;

	if (ImGui::Begin("Theme"))
	{
		for (int i = 0; i < levelManager->colorSlots.size(); i++)
		{
			if (colorSlotButton(std::string("Color Slot " + std::to_string(i)), levelManager->colorSlots[i]->currentColor, selectedSlot == i))
			{
				selectedSlot = i;
			}
		}
	}
	ImGui::End();
}

bool Theme::colorSlotButton(std::string label, Color color, bool selected)
{
	const float previewSize = ImGui::GetTextLineHeight();

	ImU32 inactiveCol = ImGui::GetColorU32(ImGuiCol_Button);
	ImU32 activeCol = ImGui::GetColorU32(ImGuiCol_ButtonActive);

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	float availX = ImGui::GetContentRegionAvailWidth();

	ImVec2 btnSize = ImVec2(availX, previewSize);
	ImVec2 btnMax = ImVec2(cursorPos.x + availX, cursorPos.y + previewSize);

	const char* labelPtr = label.c_str();
	bool clicked = ImGui::InvisibleButton(labelPtr, btnSize);

	bool active = clicked || selected || ImGui::IsItemHovered() || ImGui::IsItemActive();

	drawList->AddRectFilled(cursorPos, btnMax, active ? activeCol : inactiveCol);
	drawList->AddText(ImVec2(cursorPos.x + previewSize + 2.0f, cursorPos.y), ImGui::GetColorU32(ImGuiCol_Text), labelPtr, labelPtr + label.size());
	drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + previewSize, cursorPos.y + previewSize), ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, 1.0f)));

	return clicked;
}
