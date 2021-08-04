#include "Timeline.h"
#include "LevelManager.h"

Timeline::Timeline(LevelManager* levelManager)
{
	this->levelManager = levelManager;

	startTime = 0.0f;
	endTime = 30.0f;

	ImGuiController::onLayout.push_back(std::bind(&Timeline::onLayout, this));
}

void Timeline::onLayout()
{
	// Open a sequence window
	if (ImGui::Begin("Timeline"))
	{
		const int binCount = 10;
		const float binHeight = 20.0f;

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 availRegion = ImGui::GetContentRegionAvail();

		float timelineHeight = binCount * binHeight;

		ImVec2 clipSize = ImVec2(availRegion.x, timelineHeight);

		// Draw editor bins
		drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + clipSize.x, cursorPos.y + clipSize.y));

		for (int i = 0; i < binCount; i++)
		{
			ImU32 binCol;
			if (i % 2 == 0)
			{
				binCol = ImGui::GetColorU32(ImVec4(0.120f, 0.120f, 0.120f, 1.000f));
			}
			else
			{
				binCol = ImGui::GetColorU32(ImVec4(0.180f, 0.180f, 0.180f, 1.000f));
			}

			drawList->AddRectFilled(
				ImVec2(cursorPos.x, cursorPos.y + i * binHeight),
				ImVec2(cursorPos.x + availRegion.x, cursorPos.y + (i + 1) * binHeight),
				binCol);
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
			float startPos = (levelObject->startTime - startTime) / (endTime - startTime) * availRegion.x;
			float endPos = (levelObject->killTime - startTime) / (endTime - startTime) * availRegion.x;

			// Calculate strip params
			ImVec2 stripMin = ImVec2(cursorPos.x + startPos, cursorPos.y + levelObject->editorBinIndex * binHeight);
			ImVec2 stripMax = ImVec2(cursorPos.x + endPos, cursorPos.y + (levelObject->editorBinIndex + 1) * binHeight);

			ImVec2 stripSize = ImVec2(stripMax.x - stripMin.x, stripMax.y - stripMin.y);

			drawList->PushClipRect(stripMin, stripMax);

			int id = i + 1;

			ImGui::PushID(id);

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
					float timeDelta = (delta.x / availRegion.x) * (endTime - startTime);

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

				// Changing editor bins
				if (ImGui::IsWindowFocused())
				{
					levelObject->editorBinIndex -= io.MouseWheel;
					levelObject->editorBinIndex = std::clamp(levelObject->editorBinIndex, 0, binCount - 1);
				}
			}

			ImGui::PopID();

			// Draw the strip
			const char* name = levelObject->name.c_str();

			ImVec2 textSize = ImGui::CalcTextSize(name);

			ImU32 stripCol = stripActive ? activeCol : inactiveCol;

			const float distFromHead = 8.0f;

			ImVec2 localRectMin = ImVec2(stripMin.x + distFromHead, stripMin.y);
			ImVec2 localRectMax = ImVec2(stripMin.x + distFromHead + 4.0f + textSize.x, stripMax.y);

			drawList->AddRectFilled(stripMin, stripMax, stripCol);
			drawList->AddRectFilled(localRectMin, localRectMax, activeCol);
			drawList->AddText(ImVec2(localRectMin.x + 2.0f, localRectMin.y), inactiveCol, name);

			drawList->PopClipRect();
		}

		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !
			atLeastOneStripClicked)
		{
			levelManager->selectedObjectIndex = -1;
		}

		ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);

		// Draw time pointer
		float pointerPos = cursorPos.x + (levelManager->time - startTime) / (endTime - startTime) * availRegion.x;
		drawList->AddLine(ImVec2(pointerPos, cursorPos.y), ImVec2(pointerPos, cursorPos.y + timelineHeight), borderCol);

		// Frames
		drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availRegion.x, cursorPos.y + timelineHeight), borderCol);

		drawList->PopClipRect();

		ImGui::End();
	}
}
