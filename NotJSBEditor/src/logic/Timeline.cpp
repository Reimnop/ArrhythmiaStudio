#include "Timeline.h"
#include "LevelManager.h"

#include <functional>
#include <algorithm>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <Windows.h>

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

void Timeline::onLayout()
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
		drawList->PushClipRect(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y), true);

		for (int i = 0; i < EDITOR_TIMELINE_BIN_COUNT; i++)
		{
			ImVec2 binMin = ImVec2(timelineMin.x, timelineMin.y + i * EDITOR_BIN_HEIGHT);
			ImVec2 binSize = ImVec2(availX, EDITOR_BIN_HEIGHT);

			drawList->AddRectFilled(
				binMin,
				ImVec2(binMin.x + binSize.x, binMin.y + binSize.y),
				i % 2 ? EDITOR_BIN_SECONDARY_COL : EDITOR_BIN_PRIMARY_COL);
		}

		// Draw editor strips
		bool atLeastOneStripClicked = false;
		for (int i = 0; i < levelManager->levelObjects.size(); i++)
		{
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

			drawList->PushClipRect(stripMin, stripMax, true);

			ImGui::PushID(i + 1);

			ImGui::SetCursorScreenPos(stripMin);
			if (ImGui::InvisibleButton("##Strip", stripSize))
			{
				atLeastOneStripClicked = true;
				levelManager->selectedObjectIndex = i;

				Properties::inst->reset();
			}

			bool stripActive = false;

			if (ImGui::IsItemHovered())
			{
				stripActive = true;
			}

			if (ImGui::IsItemActive())
			{
				stripActive = true;

				// Dragging the strip
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

				// Copying the strip
				if (ImGui::IsWindowFocused() && ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_C))
				{
					bool clipOpenStatus = OpenClipboard(NULL);
					assert(clipOpenStatus);

					bool emptyStatus = EmptyClipboard();
					assert(emptyStatus);

					std::string jsonStr = levelObject->toJson().dump();
					const char* jsonPtr = jsonStr.c_str();

					HGLOBAL pGlobal = GlobalAlloc(GMEM_FIXED, jsonStr.size() + 1);
					memcpy(pGlobal, jsonPtr, jsonStr.size() + 1);

					SetClipboardData(EDITOR_FORMAT_OBJECT, pGlobal);

					CloseClipboard();

					GlobalFree(pGlobal);
				}
			}

			ImGui::PopID();

			// Draw the strip
			const char* name = levelObject->name.c_str();

			ImVec2 textSize = ImGui::CalcTextSize(name, name + levelObject->name.length());

			ImU32 stripCol = stripActive ? EDITOR_STRIP_ACTIVE_COL : EDITOR_STRIP_INACTIVE_COL;

			ImVec2 localRectMin = ImVec2(stripMin.x + EDITOR_STRIP_LEFT, stripMin.y);
			ImVec2 localRectMax = ImVec2(stripMin.x + EDITOR_STRIP_LEFT + EDITOR_STRIP_RIGHT + textSize.x, stripMax.y);

			drawList->AddRectFilled(stripMin, stripMax, stripCol);
			drawList->AddRectFilled(localRectMin, localRectMax, EDITOR_STRIP_ACTIVE_COL);
			drawList->AddText(ImVec2(localRectMin.x + EDITOR_STRIP_TEXT_LEFT_MARGIN, localRectMin.y), EDITOR_STRIP_INACTIVE_COL, name, name + levelObject->name.size());

			drawList->PopClipRect();
		}

		// Frames
		ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
		drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + timelineHeight), borderCol);

		drawList->PopClipRect();

		// Time pointer
		drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + timelineHeight + EDITOR_TIME_POINTER_HEIGHT), true);

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

		// Object delete
		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
		{
			levelManager->removeObject(levelManager->levelObjects[levelManager->selectedObjectIndex]);

			levelManager->selectedObjectIndex = -1;
		}

		// Object paste
		if (ImGui::IsWindowFocused() && ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_V))
		{
			if (IsClipboardFormatAvailable(EDITOR_FORMAT_OBJECT))
			{
				bool openStatus = OpenClipboard(NULL);
				assert(openStatus);

				HGLOBAL pGlobal = GetClipboardData(EDITOR_FORMAT_OBJECT);

				if (pGlobal)
				{
					const char* jsonPtr = (const char*)GlobalLock(pGlobal);
					if (jsonPtr)
					{
						std::string jsonStr(jsonPtr);

						nlohmann::json objJson = nlohmann::json::parse(jsonPtr);

						float length = objJson["kill"].get<float>() - objJson["start"].get<float>();
						objJson["start"] = levelManager->time;
						objJson["kill"] = levelManager->time + length;

						LevelObject* pasteObj = new LevelObject(objJson);

						levelManager->insertObject(pasteObj);

						GlobalUnlock(pGlobal);
					}
				}

				CloseClipboard();
			}
		}

		// Jumping the time pointer
		ImGui::SetCursorScreenPos(cursorPos);
		bool pointerBeingDragged = false;
		if (ImGui::InvisibleButton("##TimePointer", ImVec2(availX, EDITOR_TIME_POINTER_HEIGHT)))
		{
			float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
			newTime = std::max(0.0f, newTime);

			levelManager->update(newTime);
			pointerBeingDragged = true;
		}

		// Dragging time pointer
		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
			newTime = std::max(0.0f, newTime);

			levelManager->update(newTime);
			pointerBeingDragged = true;
		}

		// Deselect
		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !atLeastOneStripClicked && !pointerBeingDragged)
		{
			levelManager->selectedObjectIndex = -1;
			Properties::inst->reset();
		}

		// New object popup
		ImGui::SetCursorScreenPos(timelineMin);
		ImGui::InvisibleButton("##NewObjectBtn", clipSize);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("new-popup");
		}

		if (ImGui::BeginPopup("new-popup"))
		{
			if (ImGui::Selectable("New Object"))
			{
				LevelObject* newObject = new LevelObject();
				newObject->name = "Untitled object";
				newObject->startTime = levelManager->time;
				newObject->killTime = levelManager->time + 5.0f;
				newObject->editorBinIndex = 0;

				levelManager->insertObject(newObject);
			}

			ImGui::EndPopup();
		}

		// Reset cursor
		ImGui::SetCursorScreenPos(cursorPos);
		ImGui::ItemSize(ImVec2(availX, timelineHeight + EDITOR_TIME_POINTER_HEIGHT));
	}
	ImGui::End();
}
