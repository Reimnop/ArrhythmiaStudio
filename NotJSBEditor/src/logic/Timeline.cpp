#include "Timeline.h"
#include "LevelManager.h"

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

void Timeline::onLayout()
{
	LevelManager* levelManager = LevelManager::inst;

	// Open a sequence window
	if (ImGui::Begin("Timeline"))
	{
		const int binCount = 10;
		const float timePointerHeight = 30.0f;
		const float binHeight = 20.0f;

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 availRegion = ImGui::GetContentRegionAvail();

		ImVec2 timelineMin = ImVec2(cursorPos.x, cursorPos.y + timePointerHeight);
		float timelineHeight = binCount * binHeight;

		ImVec2 clipSize = ImVec2(availRegion.x, timelineHeight);

		// Draw editor bins
		drawList->PushClipRect(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y));

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
				ImVec2(timelineMin.x, timelineMin.y + i * binHeight),
				ImVec2(timelineMin.x + availRegion.x, timelineMin.y + (i + 1) * binHeight),
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
			ImVec2 stripMin = ImVec2(timelineMin.x + startPos, timelineMin.y + levelObject->editorBinIndex * binHeight);
			ImVec2 stripMax = ImVec2(timelineMin.x + endPos, timelineMin.y + (levelObject->editorBinIndex + 1) * binHeight);

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

			ImVec2 textSize = ImGui::CalcTextSize(name, name + levelObject->name.length());

			ImU32 stripCol = stripActive ? activeCol : inactiveCol;

			const float distFromHead = 8.0f;

			ImVec2 localRectMin = ImVec2(stripMin.x + distFromHead, stripMin.y);
			ImVec2 localRectMax = ImVec2(stripMin.x + distFromHead + 4.0f + textSize.x, stripMax.y);

			drawList->AddRectFilled(stripMin, stripMax, stripCol);
			drawList->AddRectFilled(localRectMin, localRectMax, activeCol);
			drawList->AddText(ImVec2(localRectMin.x + 2.0f, localRectMin.y), inactiveCol, name, name + levelObject->name.size());

			drawList->PopClipRect();
		}

		// Frames
		ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
		drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availRegion.x, timelineMin.y + timelineHeight), borderCol);

		drawList->PopClipRect();

		// Time pointer
		drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + availRegion.x, cursorPos.y + timelineHeight + timePointerHeight));

		// Draw frame
		drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availRegion.x, cursorPos.y + timePointerHeight), borderCol);

		// Draw time pointer
		const float pointerWidth = 18.0f;
		const float pointerHeight = 22.0f;

		float pointerPos = cursorPos.x + (levelManager->time - startTime) / (endTime - startTime) * availRegion.x;
		drawList->AddLine(ImVec2(pointerPos, cursorPos.y), ImVec2(pointerPos, cursorPos.y + timelineHeight + timePointerHeight), borderCol);

		drawList->AddRectFilled(
			ImVec2(pointerPos - pointerWidth * 0.5f, cursorPos.y),
			ImVec2(pointerPos + pointerWidth * 0.5f, cursorPos.y + pointerHeight), 
			borderCol);

		drawList->AddTriangleFilled(
			ImVec2(pointerPos - pointerWidth * 0.5f, cursorPos.y + pointerHeight),
			ImVec2(pointerPos, cursorPos.y + timePointerHeight),
			ImVec2(pointerPos + pointerWidth * 0.5f, cursorPos.y + pointerHeight),
			borderCol);

		drawList->PopClipRect();

		ImGui::SetCursorScreenPos(cursorPos);

		bool pointerBeingDragged = false;
		if (ImGui::InvisibleButton("##TimePointer", ImVec2(availRegion.x, timePointerHeight)))
		{
			float newTime = startTime + (io.MousePos.x - cursorPos.x) / availRegion.x * (endTime - startTime);
			newTime = std::max(0.0f, newTime);

			levelManager->update(newTime);
			pointerBeingDragged = true;
		}
		
		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			float newTime = startTime + (io.MousePos.x - cursorPos.x) / availRegion.x * (endTime - startTime);
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
		ImGui::ItemSize(ImVec2(availRegion.x, timelineHeight + timePointerHeight));
	}
	ImGui::End();
}
