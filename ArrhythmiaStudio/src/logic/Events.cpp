#include "Events.h"

#include "../rendering/ImGuiController.h"
#include "LevelEvent.h"
#include "LevelManager.h"
#include "GlobalConstants.h"
#include "animation/Easing.h"

#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "UndoRedoManager.h"
#include "imgui/imgui_editorlib.h"
#include "undo_commands/AddLevelEventCmd.h"
#include "undo_commands/EventKeyframeAddCmd.h"
#include "undo_commands/EventKeyframeEditCmd.h"
#include "undo_commands/EventKeyframeRemoveCmd.h"

Events::Events()
{
	startTime = 0.0f;
	endTime = 30.0f;

	ImGuiController::onLayout.push_back(std::bind(&Events::onLayout, this));
}

void Events::reset()
{
	selectedEvent = nullptr;
	selectedKeyframeIndex = -1;
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
			insertEventSelectable(LevelEventType_BloomIntensity, 0.0f);
			insertEventSelectable(LevelEventType_BloomScatter, 0.9f);
			insertEventSelectable(LevelEventType_BloomThreshold, 0.8f);

			ImGui::EndPopup();
		}

		float windowWith = ImGui::GetContentRegionAvailWidth();
		if (ImGui::BeginChild("##LevelEvents", ImVec2(windowWith, 240.0f), true))
		{
			for (int i = 0; i < levelManager->level->levelEvents.size(); i++)
			{
				LevelEvent* levelEvent = levelManager->level->levelEvents[i];
				std::string label = getEventName(levelEvent->type);

				if (ImGui::Selectable(label.c_str(), selectedEvent == levelEvent))
				{
					selectedEvent = levelEvent;
					selectedKeyframeIndex = -1;
				}
			}
		}
		ImGui::EndChild();

		if (selectedEvent != nullptr)
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
				for (int i = 0; i < selectedEvent->keyframes.size(); i++)
				{
					Keyframe& kf = selectedEvent->keyframes[i];

					ImVec2 kfPos = ImVec2(
						EDITOR_KEYFRAME_OFFSET + timelineMin.x + (kf.time - startTime) / (endTime - startTime) * availX,
						timelineMin.y + EDITOR_BIN_HEIGHT * 0.5f);

					int id = i + 1;
					ImGui::PushID(id);

					ImVec2 btnMin = ImVec2(kfPos.x - EDITOR_KEYFRAME_SIZE * 0.5f, timelineMin.y);
					ImVec2 btnMax = ImVec2(btnMin.x + EDITOR_KEYFRAME_SIZE, btnMin.y + EDITOR_BIN_HEIGHT);

					bool kfHovered = ImGui::IntersectAABB(btnMin, btnMax, io.MousePos) && ImGui::IsWindowHovered();

					if (kfHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						ImGui::SetActiveID(id, ImGui::GetCurrentWindow());

						selectedKeyframeIndex = i;
					}

					bool kfActive = kfHovered;

					if (ImGui::GetActiveID() == id && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
					{
						if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
						}

						if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
						{
							selectedEvent->sequence->eraseKeyframe(kf);

							// Dragging
							ImVec2 delta = io.MouseDelta;
							float timeDelta = (delta.x / availX) * (endTime - startTime);

							kf.time += timeDelta;
							kf.time = std::max(kf.time, 0.0f);

							selectedEvent->sequence->insertKeyframe(kf);

							levelManager->updateLevelEvent(selectedEvent);
						}
					}

					if (selectedKeyframeIndex == i)
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
					const float kfTime = startTime + ((io.MousePos.x - btnMinX) / availX) * (endTime - startTime);

					if (kfTime > 0.0f)
					{
						const Keyframe kf = Keyframe(kfTime, 0.0f);

						selectedEvent->insertKeyframe(kf);
						levelManager->updateLevelEvent(selectedEvent);

						UndoRedoManager::inst->push(new EventKeyframeAddCmd(selectedEvent->type, kf), [this]() { selectedKeyframeIndex = -1; });
					}
				}

				// Draw timeline borders
				ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
				drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + EDITOR_BIN_HEIGHT),
				                  borderCol);
				drawList->PopClipRect();

				// Time pointer input handling
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

				// Reset cursor
				ImGui::SetCursorScreenPos(cursorPos);
				ImGui::ItemSize(ImVec2(availX, EDITOR_BIN_HEIGHT + EDITOR_TIME_POINTER_HEIGHT));
			}

			ImGui::Separator();
			if (selectedKeyframeIndex != -1 && selectedEvent != nullptr)
			{
				Keyframe& kf = selectedEvent->keyframes[selectedKeyframeIndex];
				Keyframe kfOldState = kf;

				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
				{
					UndoRedoManager::inst->push(new EventKeyframeRemoveCmd(selectedEvent->type, kf), nullptr, [this]() { selectedKeyframeIndex = -1; });

					selectedEvent->eraseKeyframe(kf);
					levelManager->updateLevelEvent(selectedEvent);

					selectedKeyframeIndex = -1;
				}
				else
				{
					bool kfChanged = false;
					bool kfRecordUndo = false;
					bool kfPushUndo = false;
					ImGui::DragFloat("Keyframe Time", &kf.time, 0.1f, 0.0f, levelManager->audioClip->getLength());
					kfChanged = kfChanged || ImGui::IsItemEdited();
					kfRecordUndo = kfRecordUndo || ImGui::IsItemActivated();
					kfPushUndo = kfPushUndo || ImGui::IsItemDeactivatedAfterEdit();

					ImGui::Checkbox("Keyframe Random", &kf.random);
					kfChanged = kfChanged || ImGui::IsItemEdited();
					kfRecordUndo = kfRecordUndo || ImGui::IsItemActivated();
					kfPushUndo = kfPushUndo || ImGui::IsItemDeactivatedAfterEdit();
					if (kf.random)
					{
						ImGui::DragFloat("Keyframe Min Value", &kf.values[0], 0.1f);
						kfChanged = kfChanged || ImGui::IsItemEdited();
						kfRecordUndo = kfRecordUndo || ImGui::IsItemActivated();
						kfPushUndo = kfPushUndo || ImGui::IsItemDeactivatedAfterEdit();
						ImGui::DragFloat("Keyframe Max Value", &kf.values[1], 0.1f);
						kfChanged = kfChanged || ImGui::IsItemEdited();
						kfRecordUndo = kfRecordUndo || ImGui::IsItemActivated();
						kfPushUndo = kfPushUndo || ImGui::IsItemDeactivatedAfterEdit();
					}
					else
					{
						ImGui::DragFloat("Keyframe Value", &kf.values[0], 0.1f);
						kfChanged = kfChanged || ImGui::IsItemEdited();
						kfRecordUndo = kfRecordUndo || ImGui::IsItemActivated();
						kfPushUndo = kfPushUndo || ImGui::IsItemDeactivatedAfterEdit();
					}

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

								UndoRedoManager::inst->push(new EventKeyframeEditCmd(selectedEvent->type, kfOldState, kf));
							}
						}

						ImGui::EndCombo();
					}

					if (kfChanged)
					{
						kf.evaluateValue();

						selectedEvent->sequence->eraseKeyframe(kfOldState);
						selectedEvent->sequence->insertKeyframe(kf);

						levelManager->updateLevelEvent(selectedEvent);
					}

					if (kfRecordUndo)
					{
						kfUndoState = kfOldState;
					}

					if (kfPushUndo)
					{
						UndoRedoManager::inst->push(new EventKeyframeEditCmd(selectedEvent->type, kfUndoState, kf));
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

void Events::insertEventSelectable(LevelEventType type, float defaultValue)
{
	const std::string channelName = getEventName(type);
	Level* level = LevelManager::inst->level;

	if (!level->hasLevelEvent(type) && ImGui::Selectable(channelName.c_str()))
	{
		Keyframe first = Keyframe(0.0f, defaultValue);

		LevelEvent* levelEvent = new LevelEvent(type, 1, &first);

		level->insertLevelEvent(levelEvent);

		UndoRedoManager::inst->push(new AddLevelEventCmd(levelEvent), [this]() { reset(); });
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
	case LevelEventType_BloomIntensity:
		return "Bloom Intensity";
	case LevelEventType_BloomScatter:
		return "Bloom Scatter";
	case LevelEventType_BloomThreshold:
		return "Bloom Threshold";
	}

	return "Unknown event";
}