#include "Theme.h"

#include "../rendering/ImGuiController.h"
#include "animation/ColorChannel.h"
#include "LevelManager.h"
#include "GlobalConstants.h"
#include "animation/Easing.h"

#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "imgui/imgui_editorlib.h"

Theme::Theme()
{
	startTime = 0.0f;
	endTime = 30.0f;

	ImGuiController::onLayout.push_back(std::bind(&Theme::onLayout, this));
}

void Theme::reset()
{
	selectedSlotIndex = -1;
	selectedKeyframe.reset();
}

void Theme::onLayout()
{
	LevelManager* levelManager = LevelManager::inst;

	if (ImGui::Begin("Theme"))
	{
		float windowWith = ImGui::GetContentRegionAvailWidth();
		if (ImGui::BeginChild("##ThemeColors", ImVec2(windowWith, 240.0f), true))
		{
			for (int i = 0; i < levelManager->level->colorSlots.size(); i++)
			{
				if (colorSlotButton(std::string("Color Slot " + std::to_string(i + 1)),
				                    levelManager->level->colorSlots[i]->currentColor, selectedSlotIndex == i))
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
				drawList->PushClipRect(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y),
				                       true);

				drawList->AddRectFilled(
					ImVec2(timelineMin.x, timelineMin.y),
					ImVec2(timelineMin.x + availX, timelineMin.y + EDITOR_BIN_HEIGHT),
					EDITOR_BIN_PRIMARY_COL);

				// Timeline move and zoom
				if (ImGui::IsWindowFocused() && ImGui::IntersectAABB(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + EDITOR_BIN_HEIGHT), io.MousePos))
				{
					float songLength = levelManager->audioClip->getLength();

					float visibleLength = endTime - startTime;
					float currentPos = (startTime + endTime) * 0.5f;

					float zoom = visibleLength / songLength;

					zoom -= io.MouseWheel * 0.05f;

					if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
					{
						currentPos -= io.MouseDelta.x / availX * visibleLength;
					}

					float minZoom = 2.0f / songLength;
					zoom = std::clamp(zoom, minZoom, 1.0f);

					float newVisibleLength = zoom * songLength;

					float minPos = newVisibleLength * 0.5f;
					float maxPos = songLength - minPos;

					currentPos = std::clamp(currentPos, minPos, maxPos);

					startTime = currentPos - newVisibleLength * 0.5f;
					endTime = currentPos + newVisibleLength * 0.5f;
				}

				// Draw keyframes
				ColorSlot* slot = levelManager->level->colorSlots[selectedSlotIndex];

				for (int i = 0; i < slot->channel->keyframes.size(); i++)
				{
					ColorKeyframe kf = slot->channel->keyframes[i];

					ImVec2 kfPos = ImVec2(
						EDITOR_KEYFRAME_OFFSET + timelineMin.x + (kf.time - startTime) / (endTime - startTime) * availX,
						timelineMin.y + EDITOR_BIN_HEIGHT * 0.5f);

					ImGuiID id = kf.id;
					ImGui::PushID(id);

					ImVec2 btnMin = ImVec2(kfPos.x - EDITOR_KEYFRAME_SIZE * 0.5f, timelineMin.y);
					ImVec2 btnMax = ImVec2(btnMin.x + EDITOR_KEYFRAME_SIZE, btnMin.y + EDITOR_BIN_HEIGHT);

					bool kfHovered = ImGui::IntersectAABB(btnMin, btnMax, io.MousePos) && ImGui::IsWindowHovered();

					if (kfHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						ImGui::SetActiveID(id, ImGui::GetCurrentWindow());

						selectedKeyframe = kf;
					}

					bool kfActive = kfHovered;

					if (ImGui::GetActiveID() == id)
					{
						if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
						}

						if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
						{
							// Dragging
							ImVec2 delta = io.MouseDelta;
							float timeDelta = (delta.x / availX) * (endTime - startTime);

							kf.time += timeDelta;
							kf.time = std::max(kf.time, 0.0f);

							std::vector<ColorKeyframe>::iterator it = std::remove_if(
								slot->channel->keyframes.begin(),
								slot->channel->keyframes.end(),
								[kf](const ColorKeyframe& a)
								{
									return a.id == kf.id;
								});

							slot->channel->keyframes.erase(it);
							slot->channel->insertKeyframe(kf);

							selectedKeyframe = kf;

							levelManager->updateColorSlot(slot);
						}
					}

					if (selectedKeyframe.has_value() && selectedKeyframe.value().id == kf.id)
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

				const float btnMinX = EDITOR_KEYFRAME_OFFSET + cursorPos.x;

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

				// Draw borders of the timeline
				ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
				drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + EDITOR_BIN_HEIGHT),
				                  borderCol);
				drawList->PopClipRect();

				// Time pointer input handing
				ImGui::SetCursorScreenPos(cursorPos);
				if (ImGui::InvisibleButton("##TimePointer", ImVec2(availX, EDITOR_TIME_POINTER_HEIGHT)))
				{
					float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
					newTime = std::clamp(newTime, 0.0f, levelManager->audioClip->getLength());

					levelManager->audioClip->pause();

					levelManager->updateLevel(newTime);
					levelManager->audioClip->seek(newTime);
				}

				if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				{
					float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
					newTime = std::clamp(newTime, 0.0f, levelManager->audioClip->getLength());

					levelManager->audioClip->pause();

					levelManager->updateLevel(newTime);
					levelManager->audioClip->seek(newTime);
				}

				// Drawing the time pointer
				drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + availX,
				                                         cursorPos.y + EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT),
				                       true);

				// Draw frame
				drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT),
				                  borderCol);

				// Draw time pointer
				constexpr float pointerRectHeight = EDITOR_TIME_POINTER_HEIGHT - EDITOR_TIME_POINTER_TRI_HEIGHT;

				float pointerPos = cursorPos.x + (levelManager->time - startTime) / (endTime - startTime) * availX;

				drawList->AddLine(ImVec2(pointerPos, cursorPos.y),
				                  ImVec2(pointerPos, cursorPos.y + EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT),
				                  borderCol);

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

				// Reset cursor
				ImGui::SetCursorScreenPos(cursorPos);
				ImGui::ItemSize(ImVec2(availX, EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT));
			}

			ImGui::Separator();
			if (selectedKeyframe.has_value() && selectedSlotIndex != -1)
			{
				ColorSlot* currentSlot = levelManager->level->colorSlots[selectedSlotIndex];
				ColorKeyframe kf = selectedKeyframe.value();

				if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
				{
					std::vector<ColorKeyframe>::iterator it = std::remove_if(
						currentSlot->channel->keyframes.begin(), currentSlot->channel->keyframes.end(),
						[kf](const ColorKeyframe& a)
						{
							return a.id == kf.id;
						});

					currentSlot->channel->keyframes.erase(it);
					levelManager->updateColorSlot(currentSlot);

					selectedKeyframe.reset();
				}
				else
				{
					bool kfChanged = false;
					ImGui::DragFloat("Keyframe Time", &kf.time, 0.1f, 0.0f, levelManager->audioClip->getLength());
					kfChanged = kfChanged || ImGui::IsItemEdited();
					ImGui::ColorEdit3("Keyframe Color", &kf.color.r);
					kfChanged = kfChanged || ImGui::IsItemEdited();

					std::string currentEaseName = Easing::getEaseName(kf.easing);
					if (ImGui::BeginCombo("Easing", currentEaseName.c_str()))
					{
						for (int i = 0; i < EaseType_Count; i++)
						{
							std::string easeName = Easing::getEaseName((EaseType)i);
							if (ImGui::Selectable(easeName.c_str(), kf.easing == i))
							{
								kf.easing = (EaseType)i;
								kfChanged = true;
							}
						}

						ImGui::EndCombo();
					}

					if (kfChanged)
					{
						std::vector<ColorKeyframe>::iterator it = std::remove_if(
							currentSlot->channel->keyframes.begin(), currentSlot->channel->keyframes.end(),
							[kf](const ColorKeyframe& a)
							{
								return a.id == kf.id;
							});

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
		else
		{
			ImGui::Text("No color slot selected");
		}
	}
	ImGui::End();
}

bool Theme::colorSlotButton(std::string label, Color color, bool selected) const
{
	ImU32 inactiveCol = ImGui::GetColorU32(ImGuiCol_Button);
	ImU32 activeCol = ImGui::GetColorU32(ImGuiCol_ButtonActive);

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	float availX = ImGui::GetContentRegionAvailWidth();

	ImVec2 btnSize = ImVec2(availX, EDITOR_THEME_COLOR_SLOT_HEIGHT);
	ImVec2 btnMax = ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_THEME_COLOR_SLOT_HEIGHT);

	const char* labelPtr = label.c_str();
	bool clicked = ImGui::InvisibleButton(labelPtr, btnSize);

	bool active = clicked || selected || ImGui::IsItemHovered() || ImGui::IsItemActive();

	drawList->AddRectFilled(cursorPos, btnMax, active ? activeCol : inactiveCol);
	drawList->AddText(ImVec2(cursorPos.x + EDITOR_THEME_COLOR_SLOT_HEIGHT + 2.0f, cursorPos.y),
	                  ImGui::GetColorU32(ImGuiCol_Text),
	                  labelPtr, labelPtr + label.size());
	drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + EDITOR_THEME_COLOR_SLOT_HEIGHT,
	                                          cursorPos.y + EDITOR_THEME_COLOR_SLOT_HEIGHT),
	                        ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, 1.0f)));

	return clicked;
}