#include "Theme.h"

#include "../rendering/ImGuiController.h"
#include "animation/ColorChannel.h"
#include "LevelManager.h"
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
				if (colorSlotButton(std::string("Color Slot " + std::to_string(i + 1)), levelManager->colorSlots[i]->currentColor, selectedSlotIndex == i))
				{
					selectedSlotIndex = i;
					selectedKeyframe.reset();
				}
			}
		}
		ImGui::EndChild();

		if (selectedSlotIndex != -1)
		{
			{
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImGuiStyle& style = ImGui::GetStyle();
				ImGuiIO& io = ImGui::GetIO();

				ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				ImVec2 timelineMin = ImVec2(cursorPos.x, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT);

				float availX = ImGui::GetContentRegionAvailWidth();

				ImVec2 clipSize = ImVec2(availX, EDITOR_BIN_HEIGHT);

				// Draw timeline
				drawList->PushClipRect(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y), true);

				drawList->AddRectFilled(
					ImVec2(timelineMin.x, timelineMin.y),
					ImVec2(timelineMin.x + availX, timelineMin.y + EDITOR_BIN_HEIGHT),
					EDITOR_BIN_PRIMARY_COL);

				// Draw keyframes
				ColorSlot* slot = levelManager->colorSlots[selectedSlotIndex];

				for (int i = 0; i < slot->channel->keyframes.size(); i++)
				{
					ColorKeyframe kf = slot->channel->keyframes[i];

					ImVec2 kfPos = ImVec2(
						EDITOR_KEYFRAME_OFFSET + timelineMin.x + (kf.time - startTime) / (endTime - startTime) * availX,
						timelineMin.y + EDITOR_BIN_HEIGHT * 0.5f);

					ImGui::PushID(i + 1);

					ImVec2 btnMin = ImVec2(kfPos.x - EDITOR_KEYFRAME_SIZE * 0.5f, timelineMin.y);

					ImGui::SetCursorScreenPos(btnMin);
					if (ImGui::InvisibleButton("##Keyframe", ImVec2(EDITOR_KEYFRAME_SIZE, EDITOR_BIN_HEIGHT)))
					{
						selectedKeyframe = kf;
					}

					bool kfActive;
					if (ImGui::IsItemHovered())
					{
						kfActive = true;
					}
					else
					{
						kfActive = false;
					}

					if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					{
						selectedKeyframe = kf;

						// Dragging
						ImVec2 delta = io.MouseDelta;
						float timeDelta = (delta.x / availX) * (endTime - startTime);

						kf.time += timeDelta;
						kf.time = std::max(kf.time, 0.0f);

						std::vector<ColorKeyframe>::iterator it = std::find(
							slot->channel->keyframes.begin(),
							slot->channel->keyframes.end(),
							selectedKeyframe.value());
						slot->channel->keyframes.erase(it);

						slot->channel->insertKeyframe(kf);
						selectedKeyframe = kf;

						levelManager->updateColorSlot(slot);
					}

					if (selectedKeyframe.has_value() && selectedKeyframe.value() == kf)
					{
						kfActive = true;
					}

					ImGui::PopID();

					drawList->AddQuadFilled(
						ImVec2(kfPos.x, kfPos.y - EDITOR_KEYFRAME_SIZE * 0.5f),
						ImVec2(kfPos.x - EDITOR_KEYFRAME_SIZE * 0.5f, kfPos.y),
						ImVec2(kfPos.x, kfPos.y + EDITOR_KEYFRAME_SIZE * 0.5f),
						ImVec2(kfPos.x + EDITOR_KEYFRAME_SIZE * 0.5f, kfPos.y),
						kfActive ? EDITOR_KEYFRAME_ACTIVE_COL : EDITOR_KEYFRAME_INACTIVE_COL);
				}

				float btnMinX = EDITOR_KEYFRAME_OFFSET + cursorPos.x;

				ImGui::SetCursorScreenPos(ImVec2(btnMinX, timelineMin.y));
				ImGui::InvisibleButton("##KeyframeBin", ImVec2(availX, EDITOR_BIN_HEIGHT));

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					float kfTime = startTime + ((io.MousePos.x - btnMinX) / availX) * (endTime -
						startTime);

					if (kfTime > 0.0f)
					{
						ColorKeyframe kf = ColorKeyframe();
						kf.time = kfTime;
						kf.color = Color(1.0f, 1.0f, 1.0f);

						slot->channel->insertKeyframe(kf);
						slot->update(levelManager->time);

						selectedKeyframe = kf;
					}
				}

				// Frames
				ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
				drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + EDITOR_BIN_HEIGHT), borderCol);
				drawList->PopClipRect();

				// Time pointer
				drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT), true);

				// Draw frame
				drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT), borderCol);

				// Draw time pointer
				constexpr float pointerRectHeight = EDITOR_TIME_POINTER_HEIGHT - EDITOR_TIME_POINTER_TRI_HEIGHT;

				float relativeTime = std::max(levelManager->time, 0.0f);
				float pointerPos = cursorPos.x + (relativeTime - startTime) / (endTime - startTime) * availX;

				drawList->AddLine(ImVec2(pointerPos, cursorPos.y),
					ImVec2(pointerPos, cursorPos.y + EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT), borderCol);

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

				ImGui::SetCursorScreenPos(cursorPos);
				if (ImGui::InvisibleButton("##TimePointer", ImVec2(availX, EDITOR_TIME_POINTER_HEIGHT)))
				{
					float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
					newTime = std::max(0.0f, newTime);

					levelManager->update(newTime);
				}

				if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				{
					float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
					newTime = std::max(0.0f, newTime);

					levelManager->update(newTime);
				}

				// Reset cursor
				ImGui::SetCursorScreenPos(cursorPos);
				ImGui::ItemSize(ImVec2(availX, EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT));
			}

			ImGui::Separator();
			if (selectedKeyframe.has_value() && selectedSlotIndex != -1)
			{
				ColorSlot* currentSlot = levelManager->colorSlots[selectedSlotIndex];
				ColorKeyframe kf = selectedKeyframe.value();

				if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
				{
					std::vector<ColorKeyframe>::iterator it = std::find(
						currentSlot->channel->keyframes.begin(),
						currentSlot->channel->keyframes.end(),
						selectedKeyframe.value());
					currentSlot->channel->keyframes.erase(it);
					levelManager->updateColorSlot(currentSlot);

					selectedKeyframe.reset();
				}
				else
				{
					bool kfChanged = false;
					ImGui::DragFloat("Keyframe Time", &kf.time, 0.1f, 0.0f, INFINITY);
					kfChanged = kfChanged || ImGui::IsItemEdited();
					ImGui::ColorEdit3("Keyframe Color", &kf.color.r);
					kfChanged = kfChanged || ImGui::IsItemEdited();

					if (kfChanged)
					{
						std::vector<ColorKeyframe>::iterator it = std::find(
							currentSlot->channel->keyframes.begin(),
							currentSlot->channel->keyframes.end(),
							selectedKeyframe.value());
						currentSlot->channel->keyframes.erase(it);
						currentSlot->channel->insertKeyframe(kf);
						selectedKeyframe = kf;

						levelManager->updateColorSlot(currentSlot);
					}
				}
			}
			else
			{
				ImGui::Text("No keyframe selected");
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
	drawList->AddText(ImVec2(cursorPos.x + previewSize + 2.0f, cursorPos.y), ImGui::GetColorU32(ImGuiCol_Text),
	                  labelPtr, labelPtr + label.size());
	drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + previewSize, cursorPos.y + previewSize),
	                        ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, 1.0f)));

	return clicked;
}
