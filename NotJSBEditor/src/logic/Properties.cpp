#include "Properties.h"

#include <algorithm>
#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_internal.h>

#include "LevelManager.h"
#include "../rendering/ImGuiController.h"
#include "../rendering/MeshRenderer.h"
#include "GlobalConstants.h"
#include "ShapeManager.h"
#include "UndoRedoManager.h"
#include "animation/Easing.h"
#include "imgui/imgui_editorlib.h"
#include "undo_commands/EditObjectCmd.h"
#include "undo_commands/ChangeParentCmd.h"
#include "undo_commands/ObjectKeyframeEditCmd.h"

Properties* Properties::inst;

Properties::Properties()
{
	if (inst)
	{
		return;
	}

	inst = this;

	startTime = 0.0f;
	endTime = 10.0f;

	selectedChannel = nullptr;

	ImGuiController::onLayout.push_back(std::bind(&Properties::onLayout, this));
}

void Properties::reset()
{
	selectedKeyframe.reset();
	selectedChannel = nullptr;
}

void Properties::onLayout()
{
	LevelManager* levelManager = LevelManager::inst;

	// Open properties window
	if (ImGui::Begin("Properties"))
	{
		if (!levelManager->selectedObjects.empty())
		{
			if (levelManager->selectedObjects.size() == 1)
			{
				LevelObject* selectedObject = *levelManager->selectedObjects.begin();

				ImGui::InputText("Object Name", &selectedObject->name);

				bool objTimeRangeChanged = false;
				bool shouldRecordObjectState = false;
				bool shouldPushUndo = false;
				ImGui::DragFloat("Start Time", &selectedObject->startTime, 0.1f);
				objTimeRangeChanged = objTimeRangeChanged || ImGui::IsItemEdited();
				shouldRecordObjectState = shouldRecordObjectState || ImGui::IsItemActivated();
				shouldPushUndo = shouldPushUndo || ImGui::IsItemDeactivatedAfterEdit();

				ImGui::DragFloat("Kill Time", &selectedObject->killTime, 0.1f);
				objTimeRangeChanged = objTimeRangeChanged || ImGui::IsItemEdited();
				shouldRecordObjectState = shouldRecordObjectState || ImGui::IsItemActivated();
				shouldPushUndo = shouldPushUndo || ImGui::IsItemDeactivatedAfterEdit();

				if (objTimeRangeChanged)
				{
					levelManager->recalculateObjectAction(selectedObject);
					levelManager->recalculateActionIndex(levelManager->time);
				}

				std::vector<Shape> shapes = ShapeManager::inst->shapes;
				if (ImGui::BeginCombo("Shape", shapes[selectedObject->shapeIndex].name.c_str()))
				{
					for (int i = 0; i < shapes.size(); i++)
					{
						if (ImGui::Selectable(shapes[i].name.c_str(), i == selectedObject->shapeIndex))
						{
							LevelObjectProperties oldState = selectedObject->dumpProperties();

							selectedObject->shapeIndex = i;
							MeshRenderer* mr = (MeshRenderer*)selectedObject->node->renderer;
							mr->mesh = shapes[i].mesh;

							UndoRedoManager::inst->push(new EditObjectCmd(selectedObject, oldState, selectedObject->dumpProperties()));
						}
					}

					ImGui::EndCombo();
				}

				ImGui::SliderFloat("Depth", &selectedObject->depth, -32.0f, 32.0f);

				if (ImGui::IsItemEdited())
				{
					levelManager->updateObject(selectedObject);
				}

				int colorSlot = selectedObject->colorSlotIndex + 1;
				ImGui::SliderInt("Color slot", &colorSlot, 1, 30);
				selectedObject->colorSlotIndex = colorSlot - 1;

				shouldRecordObjectState = shouldRecordObjectState || ImGui::IsItemActivated();
				shouldPushUndo = shouldPushUndo || ImGui::IsItemDeactivatedAfterEdit();

				if (ImGui::IsItemEdited())
				{
					MeshRenderer* mr = (MeshRenderer*)selectedObject->node->renderer;
					mr->material = levelManager->level->colorSlots[selectedObject->colorSlotIndex]->material;
				}

				int editorBin = selectedObject->editorBinIndex + 1;
				ImGui::SliderInt("Editor bin", &editorBin, 1, EDITOR_TIMELINE_BIN_COUNT);
				selectedObject->editorBinIndex = editorBin - 1;

				shouldRecordObjectState = shouldRecordObjectState || ImGui::IsItemActivated();
				shouldPushUndo = shouldPushUndo || ImGui::IsItemDeactivatedAfterEdit();

				ImGui::SliderInt("Layer", &selectedObject->layer, 0, EDITOR_MAX_OBJECT_LAYER);

				shouldRecordObjectState = shouldRecordObjectState || ImGui::IsItemActivated();
				shouldPushUndo = shouldPushUndo || ImGui::IsItemDeactivatedAfterEdit();

				if (shouldRecordObjectState)
				{
					oldObjectState = selectedObject->dumpProperties();
				}

				if (shouldPushUndo)
				{
					UndoRedoManager::inst->push(new EditObjectCmd(selectedObject, oldObjectState, selectedObject->dumpProperties()));
				}

				Level* level = levelManager->level;

				if (ImGui::BeginCombo("Parent", selectedObject->parentId ? level->levelObjects[selectedObject->parentId]->name.c_str() : "None"))
				{
					if (ImGui::Selectable("None", selectedObject->parentId == 0))
					{
						LevelObject* oldParent = nullptr;
						if (selectedObject->parentId)
						{
							oldParent = level->levelObjects[selectedObject->parentId];
						}
						selectedObject->setParent(nullptr);

						UndoRedoManager::inst->push(new ChangeParentCmd(selectedObject, oldParent, nullptr));
					}

					for (const std::pair<uint64_t, LevelObject*> x : level->levelObjects)
					{
						LevelObject* obj = x.second;

						if (obj != selectedObject && ImGui::Selectable(obj->name.c_str(), obj->id == selectedObject->parentId))
						{
							LevelObject* oldParent = nullptr;
							if (selectedObject->parentId)
							{
								oldParent = level->levelObjects[selectedObject->parentId];
							}
							selectedObject->setParent(obj);

							UndoRedoManager::inst->push(new ChangeParentCmd(selectedObject, oldParent, obj));
						}
					}

					ImGui::EndCombo();
				}

				if (ImGui::Button("Add animation channel"))
				{
					ImGui::OpenPopup("new-channel");
				}

				if (ImGui::BeginPopup("new-channel"))
				{
					insertChannelSelectable(selectedObject, AnimationChannelType_PositionX, 0.0f);
					insertChannelSelectable(selectedObject, AnimationChannelType_PositionY, 0.0f);
					insertChannelSelectable(selectedObject, AnimationChannelType_ScaleX, 1.0f);
					insertChannelSelectable(selectedObject, AnimationChannelType_ScaleY, 1.0f);
					insertChannelSelectable(selectedObject, AnimationChannelType_Rotation, 0.0f);
					insertChannelSelectable(selectedObject, AnimationChannelType_Opacity, 1.0f);

					ImGui::EndPopup();
				}

				// Draw keyframe timelime
				{
					constexpr float timelineHeight = AnimationChannelType_Count * EDITOR_BIN_HEIGHT;

					ImDrawList* drawList = ImGui::GetWindowDrawList();
					ImGuiStyle& style = ImGui::GetStyle();
					ImGuiIO& io = ImGui::GetIO();

					const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
					ImVec2 timelineMin = ImVec2(cursorPos.x, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT);

					const float availX = ImGui::GetContentRegionAvailWidth();

					const ImVec2 clipSize = ImVec2(availX, timelineHeight);

					// Draw editor bins
					drawList->PushClipRect(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y),
						true);

					for (int i = 0; i < AnimationChannelType_Count; i++)
					{
						drawList->AddRectFilled(
							ImVec2(timelineMin.x, timelineMin.y + i * EDITOR_BIN_HEIGHT),
							ImVec2(timelineMin.x + availX, timelineMin.y + (i + 1) * EDITOR_BIN_HEIGHT),
							i % 2 ? EDITOR_BIN_SECONDARY_COL : EDITOR_BIN_PRIMARY_COL);
					}

					ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);

					// Draw channel labels
					for (int i = 0; i < selectedObject->animationChannels.size(); i++)
					{
						ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);

						std::string channelName = getChannelName(selectedObject->animationChannels[i]->type);

						ImVec2 labelMin = ImVec2(timelineMin.x + EDITOR_PROP_LABEL_TEXT_LEFT_MARGIN,
							timelineMin.y + EDITOR_BIN_HEIGHT * i);
						drawList->AddText(labelMin, textCol, channelName.c_str());
					}

					// Timeline move and zoom
					if (ImGui::IsWindowFocused() && ImGui::IntersectAABB(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y), io.MousePos))
					{
						float length = selectedObject->killTime - selectedObject->startTime;

						float visibleLength = endTime - startTime;
						float currentPos = (startTime + endTime) * 0.5f;

						float zoom = visibleLength / length;

						zoom -= io.MouseWheel * 0.05f;

						if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
						{
							currentPos -= io.MouseDelta.x / availX * visibleLength;
						}

						float minZoom = 0.25f / length;

						if (minZoom < 1.0f) 
						{
							zoom = std::clamp(zoom, minZoom, 1.0f);
						}
						else
						{
							zoom = 1.0f;
						}

						float newVisibleLength = zoom * length;

						float minPos = newVisibleLength * 0.5f;
						float maxPos = length - minPos;

						currentPos = std::clamp(currentPos, minPos, maxPos);

						startTime = currentPos - newVisibleLength * 0.5f;
						endTime = currentPos + newVisibleLength * 0.5f;
					}

					// Draw the keyframes
					int id = 0;
					for (int i = 0; i < selectedObject->animationChannels.size(); i++)
					{
						AnimationChannel* channel = selectedObject->animationChannels[i];
						Sequence* sequence = channel->sequence;

						ImVec2 binMin = ImVec2(timelineMin.x, timelineMin.y + EDITOR_BIN_HEIGHT * i);
						ImGui::SetCursorScreenPos(binMin);

						for (int j = 0; j < sequence->keyframes.size(); j++)
						{
							Keyframe kf = sequence->keyframes[j];

							ImVec2 kfPos = ImVec2(
								EDITOR_KEYFRAME_OFFSET + binMin.x + (kf.time - startTime) / (endTime - startTime) * availX,
								binMin.y + EDITOR_BIN_HEIGHT * 0.5f);

							id++;
							ImGui::PushID(id);

							ImVec2 btnMin = ImVec2(kfPos.x - EDITOR_KEYFRAME_SIZE * 0.5f, binMin.y);

							ImGui::SetCursorScreenPos(btnMin);
							if (ImGui::InvisibleButton("##Keyframe", ImVec2(EDITOR_KEYFRAME_SIZE, EDITOR_BIN_HEIGHT)))
							{
								selectedKeyframe = kf;
								selectedChannel = channel;
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
								selectedChannel = channel;

								// Dragging
								ImVec2 delta = io.MouseDelta;
								float timeDelta = (delta.x / availX) * (endTime - startTime);

								kf.time += timeDelta;
								kf.time = std::clamp(kf.time, 0.0f, selectedObject->killTime - selectedObject->startTime);

								std::vector<Keyframe>::iterator it = std::find(
									sequence->keyframes.begin(),
									sequence->keyframes.end(),
									selectedKeyframe.value());
								sequence->keyframes.erase(it);

								sequence->insertKeyframe(kf);
								selectedKeyframe = kf;

								levelManager->updateObject(selectedObject);
							}

							if (selectedKeyframe.has_value() && selectedKeyframe.value() == kf && selectedChannel ==
								channel)
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

						ImGui::PushID(channel);

						float btnMinX = EDITOR_KEYFRAME_OFFSET + binMin.x;

						ImGui::SetCursorScreenPos(ImVec2(btnMinX, binMin.y));
						ImGui::InvisibleButton("##KeyframeBin", ImVec2(availX, EDITOR_BIN_HEIGHT));

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							float kfTime = startTime + ((io.MousePos.x - btnMinX) / availX) * (endTime -
								startTime);

							if (kfTime > 0.0f)
							{
								Keyframe kf = Keyframe(kfTime, 0.0f);

								channel->sequence->insertKeyframe(kf);
								levelManager->updateObject(selectedObject);
							}
						}

						ImGui::PopID();
					}

					// Draw timeline border
					drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + timelineHeight),
						borderCol);

					drawList->PopClipRect();

					// Time pointer input handling
					ImGui::SetCursorScreenPos(cursorPos);
					if (ImGui::InvisibleButton("##TimePointer", ImVec2(availX, EDITOR_TIME_POINTER_HEIGHT)))
					{
						float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
						newTime = std::max(0.0f, newTime);
						newTime += selectedObject->startTime;

						levelManager->audioClip->pause();

						levelManager->updateLevel(newTime);
						levelManager->audioClip->seek(newTime);
					}

					if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					{
						float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
						newTime = std::max(0.0f, newTime);
						newTime += selectedObject->startTime;

						levelManager->audioClip->pause();

						levelManager->updateLevel(newTime);
						levelManager->audioClip->seek(newTime);
					}

					// Time pointer
					drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + availX,
						cursorPos.y + timelineHeight + EDITOR_TIME_POINTER_HEIGHT),
						true);

					// Draw frame
					drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT),
						borderCol);

					// Draw time pointer
					constexpr float pointerRectHeight = EDITOR_TIME_POINTER_HEIGHT - EDITOR_TIME_POINTER_TRI_HEIGHT;

					float relativeTime = std::max(levelManager->time - selectedObject->startTime, 0.0f);
					float pointerPos = cursorPos.x + (relativeTime - startTime) / (endTime - startTime) * availX;

					drawList->AddLine(ImVec2(pointerPos, cursorPos.y),
						ImVec2(pointerPos, cursorPos.y + timelineHeight + EDITOR_TIME_POINTER_HEIGHT),
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
					ImGui::ItemSize(ImVec2(availX, timelineHeight + EDITOR_TIME_POINTER_HEIGHT));
				}

				// Draw keyframe editor
				ImGui::Separator();
				if (selectedKeyframe.has_value() && selectedChannel != nullptr)
				{
					Keyframe& kf = selectedKeyframe.value();
					Keyframe oldKf = kf;

					if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
					{
						std::vector<Keyframe>::iterator it = std::find(selectedChannel->sequence->keyframes.begin(),
							selectedChannel->sequence->keyframes.end(),
							selectedKeyframe.value());
						selectedChannel->sequence->keyframes.erase(it);
						selectedChannel->update(levelManager->time);

						selectedKeyframe.reset();
						selectedChannel = nullptr;
					}
					else
					{
						bool kfChanged = false;
						ImGui::DragFloat("Keyframe Time", &kf.time, 0.1f, 0.0f, selectedObject->killTime - selectedObject->startTime);
						kfChanged = kfChanged || ImGui::IsItemEdited();

						ImGui::Checkbox("Keyframe Random", &kf.random);
						kfChanged = kfChanged || ImGui::IsItemEdited();
						if (kf.random)
						{
							ImGui::DragFloat("Keyframe Min Value", &kf.values[0], 0.1f);
							kfChanged = kfChanged || ImGui::IsItemEdited();
							ImGui::DragFloat("Keyframe Max Value", &kf.values[1], 0.1f);
							kfChanged = kfChanged || ImGui::IsItemEdited();
						}
						else
						{
							ImGui::DragFloat("Keyframe Value", &kf.values[0], 0.1f);
							kfChanged = kfChanged || ImGui::IsItemEdited();
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
								}
							}

							ImGui::EndCombo();
						}

						if (kfChanged)
						{
							Sequence* sequence = selectedChannel->sequence;

							std::vector<Keyframe>::iterator it = std::find(sequence->keyframes.begin(), sequence->keyframes.end(), oldKf);

							sequence->keyframes.erase(it);
							sequence->insertKeyframe(kf);

							levelManager->updateObject(selectedObject);
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
				ImGui::Text("Multi editing is not supported");
			}
		}
		else
		{
			ImGui::Text("No object selected");
		}
	}
	ImGui::End();
}

void Properties::insertChannelSelectable(LevelObject* levelObject, AnimationChannelType channelType, float defaultValue)
{
	const std::string channelName = getChannelName(channelType);
	if (!levelObject->hasChannel(channelType) && ImGui::Selectable(channelName.c_str()))
	{
		Keyframe first = Keyframe(0.0f, defaultValue);

		AnimationChannel* channel = new AnimationChannel(channelType, 1, &first);

		levelObject->insertChannel(channel);
	}
}

std::string Properties::getChannelName(AnimationChannelType channelType) const
{
	switch (channelType)
	{
	case AnimationChannelType_PositionX:
		return "Position X";
	case AnimationChannelType_PositionY:
		return "Position Y";
	case AnimationChannelType_ScaleX:
		return "Scale X";
	case AnimationChannelType_ScaleY:
		return "Scale Y";
	case AnimationChannelType_Rotation:
		return "Rotation";
	case AnimationChannelType_Opacity:
		return "Opacity";
	}

	return "Unknown type";
}
