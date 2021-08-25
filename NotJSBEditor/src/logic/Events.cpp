#include "Events.h"

#include "../rendering/ImGuiController.h"
#include "LevelEvent.h"
#include "LevelManager.h"
#include "GlobalConstants.h"
#include "animation/Easing.h"

#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

Events::Events()
{
	startTime = 0.0f;
	endTime = 30.0f;

	ImGuiController::onLayout.push_back(std::bind(&Events::onLayout, this));
}

void Events::reset()
{
	selectedEventIndex = -1;
	selectedKeyframe.reset();
}

void Events::onLayout()
{
	LevelManager* levelManager = LevelManager::inst;

	if (ImGui::Begin("Events"))
	{
		if (ImGui::Button("Add level event"))
		{
			ImGui::OpenPopup("add-level-event-popup");
		}

		if (ImGui::BeginPopup("add-level-event-popup"))
		{
			insertEventSelectable(LevelEventType_CameraPositionX, 0.0f);
			insertEventSelectable(LevelEventType_CameraPositionY, 0.0f);
			insertEventSelectable(LevelEventType_CameraScale, 5.0f);
			insertEventSelectable(LevelEventType_CameraRotation, 0.0f);

			ImGui::EndPopup();
		}

		float windowWith = ImGui::GetContentRegionAvailWidth();
		if (ImGui::BeginChild("##LevelEvents", ImVec2(windowWith, 240.0f), true))
		{
			for (int i = 0; i < levelManager->level->levelEvents.size(); i++)
			{
				std::string label = getEventName(levelManager->level->levelEvents[i]->type);

				if (ImGui::Selectable(label.c_str(), selectedEventIndex == i))
				{
					selectedEventIndex = i;
					selectedKeyframe.reset();
				}
			}
		}
		ImGui::EndChild();

		if (selectedEventIndex != -1)
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

				// Draw keyframes
				LevelEvent* selectedEvent = levelManager->level->levelEvents[selectedEventIndex];

				for (int i = 0; i < selectedEvent->sequence->keyframes.size(); i++)
				{
					Keyframe kf = selectedEvent->sequence->keyframes[i];

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

						std::vector<Keyframe>::iterator it = std::find(
							selectedEvent->sequence->keyframes.begin(),
							selectedEvent->sequence->keyframes.end(),
							selectedKeyframe.value());
						selectedEvent->sequence->keyframes.erase(it);

						selectedEvent->sequence->insertKeyframe(kf);
						selectedKeyframe = kf;

						levelManager->updateLevelEvent(selectedEvent);
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
						Keyframe kf = Keyframe();
						kf.time = kfTime;
						kf.value = 0.0f;

						selectedEvent->sequence->insertKeyframe(kf);
						selectedEvent->update(levelManager->time);

						selectedKeyframe = kf;
					}
				}

				// Timeline move and zoom
				ImGui::SetCursorScreenPos(timelineMin);
				ImGui::InvisibleButton("##TimelineFillButton", clipSize);

				if (ImGui::IsWindowFocused() && ImGui::IsItemHovered())
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

				// Frames
				ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
				drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + EDITOR_BIN_HEIGHT),
				                  borderCol);
				drawList->PopClipRect();

				// Time pointer
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

				// Reset cursor
				ImGui::SetCursorScreenPos(cursorPos);
				ImGui::ItemSize(ImVec2(availX, EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT));
			}

			ImGui::Separator();
			if (selectedKeyframe.has_value() && selectedEventIndex != -1)
			{
				LevelEvent* selectedEvent = levelManager->level->levelEvents[selectedEventIndex];
				Keyframe kf = selectedKeyframe.value();

				if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
				{
					std::vector<Keyframe>::iterator it = std::find(
						selectedEvent->sequence->keyframes.begin(),
						selectedEvent->sequence->keyframes.end(),
						selectedKeyframe.value());
					selectedEvent->sequence->keyframes.erase(it);
					levelManager->updateLevelEvent(selectedEvent);

					selectedKeyframe.reset();
				}
				else
				{
					bool kfChanged = false;
					ImGui::DragFloat("Keyframe Time", &kf.time, 0.1f, 0.0f, levelManager->audioClip->getLength());
					kfChanged = kfChanged || ImGui::IsItemEdited();
					ImGui::DragFloat("Keyframe Value", &kf.value, 0.1f);
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
						std::vector<Keyframe>::iterator it = std::find(
							selectedEvent->sequence->keyframes.begin(),
							selectedEvent->sequence->keyframes.end(),
							selectedKeyframe.value());
						selectedEvent->sequence->keyframes.erase(it);
						selectedEvent->sequence->insertKeyframe(kf);
						selectedKeyframe = kf;

						levelManager->updateLevelEvent(selectedEvent);
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
			ImGui::Text("No level event selected");
		}
	}
	ImGui::End();
}

void Events::insertEventSelectable(LevelEventType type, float defaultValue) const
{
	const std::string channelName = getEventName(type);
	Level* level = LevelManager::inst->level;

	if (!level->hasLevelEvent(type) && ImGui::Selectable(channelName.c_str()))
	{
		Keyframe first = Keyframe();
		first.time = 0.0f;
		first.value = defaultValue;

		LevelEvent* levelEvent = new LevelEvent(type, 1, &first);

		level->insertLevelEvent(levelEvent);
	}
}

std::string Events::getEventName(LevelEventType type) const
{
	switch (type)
	{
	case LevelEventType_CameraPositionX:
		return "Camera Position X";
	case LevelEventType_CameraPositionY:
		return "Camera Position Y";
	case LevelEventType_CameraScale:
		return "Camera Scale";
	case LevelEventType_CameraRotation:
		return "Camera Rotation";
	}

	return "Unknown event";
}

float Events::lerp(float a, float b, float t)
{
	return (a * (1.0f - t)) + (b * t);
}
