#include "Theme.h"

#include "../rendering/ImGuiController.h"
#include "GlobalConstants.h"

#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


Theme::Theme()
{
	startTime = 0.0f;
	endTime = 30.0f;

	ImGuiController::onLayout.push_back(std::bind(&Theme::onLayout, this));
}

void Theme::onLayout()
{
	LevelManager* levelManager = LevelManager::inst;

	if (ImGui::Begin("Theme"))
	{
		float windowWith = ImGui::GetContentRegionAvailWidth();
		if (ImGui::BeginChild("##ThemeColors", ImVec2(windowWith, 280.0f), true))
		{
			for (int i = 0; i < levelManager->colorSlots.size(); i++)
			{
				if (colorSlotButton(std::string("Color Slot " + std::to_string(i)),
				                    levelManager->colorSlots[i]->currentColor, selectedSlot == i))
				{
					selectedSlot = i;
				}
			}
		}
		ImGui::EndChild();

		if (selectedSlot != -1)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImGuiStyle& style = ImGui::GetStyle();
			ImGuiIO& io = ImGui::GetIO();

			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			float availX = ImGui::GetContentRegionAvailWidth();

			ImVec2 clipSize = ImVec2(availX, EDITOR_BIN_HEIGHT);

			// Draw timeline
			drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + clipSize.x, cursorPos.y + clipSize.y));

			drawList->AddRectFilled(
				ImVec2(cursorPos.x, cursorPos.y),
				ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_BIN_HEIGHT),
				EDITOR_BIN_PRIMARY_COL);

			// Draw keyframes
			ColorChannel* channel = levelManager->colorSlots[selectedSlot]->channel;

			for (int i = 0; i < channel->keyframes.size(); i++)
			{
				ColorKeyframe kf = channel->keyframes[i];

				ImVec2 kfPos = ImVec2(
					EDITOR_KEYFRAME_OFFSET + cursorPos.x + (kf.time - startTime) / (endTime - startTime) * availX,
					cursorPos.y + EDITOR_BIN_HEIGHT * 0.5f);

				drawList->AddQuadFilled(
					ImVec2(kfPos.x, kfPos.y - EDITOR_KEYFRAME_SIZE * 0.5f),
					ImVec2(kfPos.x - EDITOR_KEYFRAME_SIZE * 0.5f, kfPos.y),
					ImVec2(kfPos.x, kfPos.y + EDITOR_KEYFRAME_SIZE * 0.5f),
					ImVec2(kfPos.x + EDITOR_KEYFRAME_SIZE * 0.5f, kfPos.y),
					false ? EDITOR_KEYFRAME_ACTIVE_COL : EDITOR_KEYFRAME_INACTIVE_COL);
			}

			// Frames
			ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
			drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_BIN_HEIGHT),
			                  borderCol);

			drawList->PopClipRect();

			// Reset cursor
			ImGui::SetCursorScreenPos(cursorPos);
			ImGui::ItemSize(ImVec2(availX, EDITOR_BIN_HEIGHT));
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
	drawList->AddText(ImVec2(cursorPos.x + previewSize + 2.0f, cursorPos.y), ImGui::GetColorU32(ImGuiCol_Text),
	                  labelPtr, labelPtr + label.size());
	drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + previewSize, cursorPos.y + previewSize),
	                        ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, 1.0f)));

	return clicked;
}
