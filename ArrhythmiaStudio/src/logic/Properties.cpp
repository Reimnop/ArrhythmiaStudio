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
#include "undo_commands/ObjectAddChannelCmd.h"
#include "undo_commands/ObjectAddKeyframeCmd.h"
#include "undo_commands/ObjectKeyframeEditCmd.h"
#include "undo_commands/ObjectRemoveKeyframeCmd.h"

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
	selectedKeyframeIndex = -1;
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

				ImGui::Checkbox("Is Text", &selectedObject->isText);

				shouldRecordObjectState = shouldRecordObjectState || ImGui::IsItemActivated();
				shouldPushUndo = shouldPushUndo || ImGui::IsItemDeactivatedAfterEdit();

				if (ImGui::IsItemEdited())
				{
					if (selectedObject->isText)
					{
						TextRenderer* renderer = new TextRenderer(LevelManager::inst->mainFont);
						renderer->material = LevelManager::inst->level->colorSlots[selectedObject->colorSlotIndex]->material;
						renderer->setText(selectedObject->text);

						selectedObject->node->renderer = renderer;
					}
					else
					{
						MeshRenderer* renderer = new MeshRenderer();
						renderer->mesh = ShapeManager::inst->shapes[selectedObject->shapeIndex].mesh;
						renderer->material = LevelManager::inst->level->colorSlots[selectedObject->colorSlotIndex]->material;
						renderer->shader = LevelManager::inst->unlitShader;

						selectedObject->node->renderer = renderer;
					}
				}

				if (selectedObject->isText)
				{
					ImGui::InputTextMultiline("Text", &selectedObject->text);

					shouldRecordObjectState = shouldRecordObjectState || ImGui::IsItemActivated();
					shouldPushUndo = shouldPushUndo || ImGui::IsItemDeactivatedAfterEdit();

					if (ImGui::IsItemEdited())
					{
						TextRenderer* renderer = (TextRenderer*)selectedObject->node->renderer;
						renderer->setText(selectedObject->text);
					}
				}
				else
				{
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

								UndoRedoManager::inst->push(new EditObjectCmd(selectedObject->id, oldState, selectedObject->dumpProperties()));
							}
						}

						ImGui::EndCombo();
					}
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
					if (selectedObject->isText) 
					{
						TextRenderer* tr = (TextRenderer*)selectedObject->node->renderer;
						tr->material = levelManager->level->colorSlots[selectedObject->colorSlotIndex]->material;
					}
					else 
					{
						MeshRenderer* mr = (MeshRenderer*)selectedObject->node->renderer;
						mr->material = levelManager->level->colorSlots[selectedObject->colorSlotIndex]->material;
					}
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
					UndoRedoManager::inst->push(new EditObjectCmd(selectedObject->id, oldObjectState, selectedObject->dumpProperties()));
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

						UndoRedoManager::inst->push(new ChangeParentCmd(selectedObject->id, oldParent, nullptr));
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

							UndoRedoManager::inst->push(new ChangeParentCmd(selectedObject->id, oldParent, obj));
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

					bool timelineHovered = ImGui::IntersectAABB(timelineMin, ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y), io.MousePos) && ImGui::IsWindowHovered();

					// Timeline move and zoom
					if (ImGui::IsWindowFocused() && timelineHovered)
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

					// Deselect
					if (timelineHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						reset();
					}

					int id = 0;

					// Draw the keyframes
					for (int i = 0; i < selectedObject->animationChannels.size(); i++)
					{
						AnimationChannel* channel = selectedObject->animationChannels[i];
						Sequence* sequence = channel->sequence;

						ImVec2 binMin = ImVec2(timelineMin.x, timelineMin.y + EDITOR_BIN_HEIGHT * i);
						ImGui::SetCursorScreenPos(binMin);
						
						for (int j = 0; j < channel->keyframes.size(); j++)
						{
							Keyframe& kf = channel->keyframes[j];

							ImVec2 kfPos = ImVec2(
								EDITOR_KEYFRAME_OFFSET + binMin.x + (kf.time - startTime) / (endTime - startTime) * availX,
								binMin.y + EDITOR_BIN_HEIGHT * 0.5f);

							id++;
							ImGui::PushID(id);

							ImVec2 btnMin = ImVec2(kfPos.x - EDITOR_KEYFRAME_SIZE * 0.5f, binMin.y);
							ImVec2 btnMax = ImVec2(btnMin.x + EDITOR_KEYFRAME_SIZE, btnMin.y + EDITOR_BIN_HEIGHT);

							bool kfHovered = ImGui::IntersectAABB(btnMin, btnMax, io.MousePos) && ImGui::IsWindowHovered();

							if (kfHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								ImGui::SetActiveID(id, ImGui::GetCurrentWindow());

								selectedKeyframeIndex = j;
								selectedChannel = channel;
							}

							bool kfActive = kfHovered;

							if (ImGui::GetActiveID() == id && ImGui::IsWindowFocused())
							{
								if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) 
								{
									ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
								}

								if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
								{
									sequence->eraseKeyframe(kf);

									// Dragging
									ImVec2 delta = io.MouseDelta;
									float timeDelta = (delta.x / availX) * (endTime - startTime);

									kf.time += timeDelta;
									kf.time = std::clamp(kf.time, 0.0f, selectedObject->killTime - selectedObject->startTime);
									
									sequence->insertKeyframe(kf);

									levelManager->updateObject(selectedObject);
								}
							}

							if (selectedKeyframeIndex == j && selectedChannel == channel)
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

						bool binHovered = ImGui::IntersectAABB(ImVec2(btnMinX, binMin.y), ImVec2(btnMinX + availX, binMin.y + EDITOR_BIN_HEIGHT), io.MousePos) && ImGui::IsWindowHovered();
						if (binHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							float kfTime = startTime + ((io.MousePos.x - btnMinX) / availX) * (endTime -
								startTime);

							if (kfTime > 0.0f)
							{
								Keyframe kf = Keyframe(kfTime, 0.0f);

								channel->insertKeyframe(kf);
								levelManager->updateObject(selectedObject);

								UndoRedoManager::inst->push(new ObjectAddKeyframeCmd(selectedObject->id, channel->type, kf), [this]() { reset(); });
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
				if (selectedKeyframeIndex != -1 && selectedChannel != nullptr)
				{
					Keyframe& kf = selectedChannel->keyframes[selectedKeyframeIndex];
					Keyframe kfOldState = kf;

					if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
					{
						UndoRedoManager::inst->push(new ObjectRemoveKeyframeCmd(selectedObject->id, selectedChannel->type, kf), nullptr, [this]() { reset(); });

						selectedChannel->eraseKeyframe(kf);
						selectedChannel->update(levelManager->time);

						reset();
					}
					else
					{
						bool kfChanged = false;
						bool kfRecordUndo = false;
						bool kfPushUndo = false;

						ImGui::DragFloat("Keyframe Time", &kf.time, 0.1f, 0.0f, selectedObject->killTime - selectedObject->startTime);
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

									UndoRedoManager::inst->push(new ObjectKeyframeEditCmd(selectedObject->id, selectedChannel->type, kfOldState, kf));
								}
							}

							ImGui::EndCombo();
						}

						if (kfChanged)
						{
							Sequence* sequence = selectedChannel->sequence;

							// Re-insert for sorted list
							sequence->eraseKeyframe(kfOldState);
							sequence->insertKeyframe(kf);

							levelManager->updateObject(selectedObject);
						}

						if (kfRecordUndo)
						{
							kfUndoState = kfOldState;
						}

						if (kfPushUndo)
						{
							UndoRedoManager::inst->push(new ObjectKeyframeEditCmd(selectedObject->id, selectedChannel->type, kfUndoState, kf));
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

		UndoRedoManager::inst->push(new ObjectAddChannelCmd(levelObject->id, channel), [this]() { reset(); });
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
