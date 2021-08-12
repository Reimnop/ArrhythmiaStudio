#include "Timeline.h"
#include "LevelManager.h"

#include <functional>
#include <algorithm>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "../rendering/ImGuiController.h"
#include "GlobalConstants.h"

Timeline* Timeline::inst;

Timeline::Timeline()
{
	if (inst)
	{
		return;
	}

	inst = this;

	startTime = 0.0f;
	endTime = 30.0f;

	ImGuiController::onLayout.push_back(std::bind(&Timeline::onLayout, this));
}

void Timeline::onLayout() const
{
	LevelManager* levelManager = LevelManager::inst;

	// Open a sequence window
	if (ImGui::Begin("Timeline"))
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		float availX = ImGui::GetContentRegionAvailWidth();

		ImVec2 timelineMin = ImVec2(cursorPos.x, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT);
		float timelineHeight = EDITOR_TIMELINE_BIN_COUNT * EDITOR_BIN_HEIGHT;

		ImVec2 clipSize = ImVec2(availX, timelineHeight);

		// Draw editor bins
		drawList->PushClipRect(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y));

		for (int i = 0; i < EDITOR_TIMELINE_BIN_COUNT; i++)
		{
			drawList->AddRectFilled(
				ImVec2(timelineMin.x, timelineMin.y + i * EDITOR_BIN_HEIGHT),
				ImVec2(timelineMin.x + availX, timelineMin.y + (i + 1) * EDITOR_BIN_HEIGHT),
				i % 2 ? EDITOR_BIN_SECONDARY_COL : EDITOR_BIN_PRIMARY_COL);
		}

		// Draw editor strips
		bool atLeastOneStripClicked = false;
		for (int i = 0; i < levelManager->levelObjects.size(); i++)
		{
			const ImU32 inactiveCol = ImGui::GetColorU32(ImVec4(0.729f, 0.729f, 0.729f, 1.000f));
			const ImU32 activeCol = ImGui::GetColorU32(ImVec4(0.384f, 0.384f, 0.384f, 1.000f));

			LevelObject* levelObject = levelManager->levelObjects[i];

			if (levelObject->killTime < startTime || levelObject->startTime > endTime)
			{
				continue;
			}

			// Calculate start and end position in pixel
			float startPos = (levelObject->startTime - startTime) / (endTime - startTime) * availX;
			float endPos = (levelObject->killTime - startTime) / (endTime - startTime) * availX;

			// Calculate strip params
			ImVec2 stripMin = ImVec2(timelineMin.x + startPos,
			                         timelineMin.y + levelObject->editorBinIndex * EDITOR_BIN_HEIGHT);
			ImVec2 stripMax = ImVec2(timelineMin.x + endPos,
			                         timelineMin.y + (levelObject->editorBinIndex + 1) * EDITOR_BIN_HEIGHT);

			ImVec2 stripSize = ImVec2(stripMax.x - stripMin.x, stripMax.y - stripMin.y);

			drawList->PushClipRect(stripMin, stripMax);

			ImGui::PushID(i + 1);

			ImGui::SetCursorScreenPos(stripMin);
			if (ImGui::InvisibleButton("##Strip", stripSize))
			{
				atLeastOneStripClicked = true;
				levelManager->selectedObjectIndex = i;
			}

			bool stripActive = false;

			if (ImGui::IsItemHovered())
			{
				stripActive = true;
			}

			if (ImGui::IsItemActive())
			{
				stripActive = true;

				// Dragging
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				{
					atLeastOneStripClicked = true;
					levelManager->selectedObjectIndex = i;

					ImVec2 delta = io.MouseDelta;
					float timeDelta = (delta.x / availX) * (endTime - startTime);

					timeDelta = std::clamp(timeDelta, -levelObject->startTime, endTime - levelObject->killTime);

					levelObject->startTime += timeDelta;
					levelObject->killTime += timeDelta;

					// Recalculate object actions
					levelManager->recalculateObjectAction(levelObject);
					levelManager->recalculateActionIndex(levelManager->time);
				}
			}

			if (levelManager->selectedObjectIndex == i)
			{
				stripActive = true;
			}

			ImGui::PopID();

			// Draw the strip
			const char* name = levelObject->name.c_str();

			ImVec2 textSize = ImGui::CalcTextSize(name, name + levelObject->name.length());

			ImU32 stripCol = stripActive ? activeCol : inactiveCol;

			const float distFromHead = 8.0f;

			ImVec2 localRectMin = ImVec2(stripMin.x + distFromHead, stripMin.y);
			ImVec2 localRectMax = ImVec2(stripMin.x + distFromHead + 4.0f + textSize.x, stripMax.y);

			drawList->AddRectFilled(stripMin, stripMax, stripCol);
			drawList->AddRectFilled(localRectMin, localRectMax, activeCol);
			drawList->AddText(ImVec2(localRectMin.x + 2.0f, localRectMin.y), inactiveCol, name,
			                  name + levelObject->name.size());

			drawList->PopClipRect();
		}

		// Frames
		ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
		drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + timelineHeight), borderCol);

		drawList->PopClipRect();

		// Time pointer
		drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + availX,
		                                         cursorPos.y + timelineHeight + EDITOR_TIME_POINTER_HEIGHT));

		// Draw frame
		drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT), borderCol);

		// Draw time pointer
		constexpr float pointerRectHeight = EDITOR_TIME_POINTER_HEIGHT - EDITOR_TIME_POINTER_TRI_HEIGHT;

		float pointerPos = cursorPos.x + (levelManager->time - startTime) / (endTime - startTime) * availX;
		drawList->AddLine(ImVec2(pointerPos, cursorPos.y),
		                  ImVec2(pointerPos, cursorPos.y + timelineHeight + EDITOR_TIME_POINTER_HEIGHT), borderCol);

		drawList->AddRectFilled(
			ImVec2(pointerPos - EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y),
			ImVec2(pointerPos + EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y + pointerRectHeight),
			borderCol);

		drawList->AddTriangleFilled(
			ImVec2(pointerPos - EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y + pointerRectHeight),
			ImVec2(pointerPos, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT),
			ImVec2(pointerPos + EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y + pointerRectHeight),
			borderCol);

		drawList->PopClipRect();

		// Changing editor bins
		if (levelManager->selectedObjectIndex != -1)
		{
			LevelObject* selectedObject = levelManager->levelObjects[levelManager->selectedObjectIndex];
			if (ImGui::IsWindowHovered())
			{
				selectedObject->editorBinIndex -= io.MouseWheel;
				selectedObject->editorBinIndex = std::clamp(selectedObject->editorBinIndex, 0, EDITOR_TIMELINE_BIN_COUNT - 1);
			}
		}

		ImGui::SetCursorScreenPos(cursorPos);

		bool pointerBeingDragged = false;
		if (ImGui::InvisibleButton("##TimePointer", ImVec2(availX, EDITOR_TIME_POINTER_HEIGHT)))
		{
			float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
			newTime = std::max(0.0f, newTime);

			levelManager->update(newTime);
			pointerBeingDragged = true;
		}

		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
			newTime = std::max(0.0f, newTime);

			levelManager->update(newTime);
			pointerBeingDragged = true;
		}

		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
			!atLeastOneStripClicked && !pointerBeingDragged)
		{
			levelManager->selectedObjectIndex = -1;
		}

		// Reset cursor
		ImGui::SetCursorScreenPos(cursorPos);
		ImGui::ItemSize(ImVec2(availX, timelineHeight + EDITOR_TIME_POINTER_HEIGHT));
	}
	ImGui::End();
}
