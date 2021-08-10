#include "Properties.h"
#include "LevelManager.h"

#include "../rendering/ImGuiController.h"
#include "GlobalConstants.h"

#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_internal.h>

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

	// Open Dope Sheet window
	if (ImGui::Begin("Properties"))
	{
		if (levelManager->selectedObjectIndex != -1)
		{
			LevelObject* selectedObject = levelManager->levelObjects[levelManager->selectedObjectIndex];

			ImGui::InputText("Object Name", &selectedObject->name);

			bool objTimeRangeChanged = false;
			ImGui::DragFloat("Start Time", &selectedObject->startTime, 0.1f);
			objTimeRangeChanged = objTimeRangeChanged || ImGui::IsItemEdited();
			ImGui::DragFloat("Kill Time", &selectedObject->killTime, 0.1f);
			objTimeRangeChanged = objTimeRangeChanged || ImGui::IsItemEdited();

			if (objTimeRangeChanged)
			{
				levelManager->recalculateObjectAction(selectedObject);
				levelManager->recalculateActionIndex(levelManager->time);
			}

			ImGui::SliderInt("Editor bin", &selectedObject->editorBinIndex, 0, 9);

			if (ImGui::Button("Add animation channel"))
			{
				ImGui::OpenPopup("new-channel");
			}

			if (ImGui::BeginPopup("new-channel"))
			{
				if (!selectedObject->hasChannel(AnimationChannelType_PositionX) && ImGui::Selectable("Position X"))
				{
					Keyframe first = Keyframe();
					first.time = 0.0f;
					first.value = 0.0f;

					AnimationChannel* channel = new AnimationChannel(AnimationChannelType_PositionX, 1, &first);

					selectedObject->insertChannel(channel);
				}

				if (!selectedObject->hasChannel(AnimationChannelType_PositionY) && ImGui::Selectable("Position Y"))
				{
					Keyframe first = Keyframe();
					first.time = 0.0f;
					first.value = 0.0f;

					AnimationChannel* channel = new AnimationChannel(AnimationChannelType_PositionY, 1, &first);

					selectedObject->insertChannel(channel);
				}

				if (!selectedObject->hasChannel(AnimationChannelType_ScaleX) && ImGui::Selectable("Scale X"))
				{
					Keyframe first = Keyframe();
					first.time = 0.0f;
					first.value = 1.0f;

					AnimationChannel* channel = new AnimationChannel(AnimationChannelType_ScaleX, 1, &first);

					selectedObject->insertChannel(channel);
				}

				if (!selectedObject->hasChannel(AnimationChannelType_ScaleY) && ImGui::Selectable("Scale Y"))
				{
					Keyframe first = Keyframe();
					first.time = 0.0f;
					first.value = 1.0f;

					AnimationChannel* channel = new AnimationChannel(AnimationChannelType_ScaleY, 1, &first);

					selectedObject->insertChannel(channel);
				}

				if (!selectedObject->hasChannel(AnimationChannelType_Rotation) && ImGui::Selectable("Rotation"))
				{
					Keyframe first = Keyframe();
					first.time = 0.0f;
					first.value = 0.0f;

					AnimationChannel* channel = new AnimationChannel(AnimationChannelType_Rotation, 1, &first);

					selectedObject->insertChannel(channel);
				}

				ImGui::EndPopup();
			}

			// Draw keyframe timelime
			{
				constexpr float timelineHeight = EDITOR_PROP_BIN_COUNT * EDITOR_BIN_HEIGHT;

				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImGuiStyle& style = ImGui::GetStyle();
				ImGuiIO& io = ImGui::GetIO();

				const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				const float availX = ImGui::GetContentRegionAvailWidth();

				const ImVec2 clipSize = ImVec2(availX, timelineHeight);

				// Draw editor bins
				drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + clipSize.x, cursorPos.y + clipSize.y));

				for (int i = 0; i < EDITOR_PROP_BIN_COUNT; i++)
				{
					drawList->AddRectFilled(
						ImVec2(cursorPos.x, cursorPos.y + i * EDITOR_BIN_HEIGHT),
						ImVec2(cursorPos.x + availX, cursorPos.y + (i + 1) * EDITOR_BIN_HEIGHT),
						i % 2 ? EDITOR_BIN_SECONDARY_COL : EDITOR_BIN_PRIMARY_COL);
				}

				ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);

				// Draw channel labels
				for (int i = 0; i < selectedObject->animationChannels.size(); i++)
				{
					ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);

					ImVec2 labelMin = ImVec2(cursorPos.x + 2.0f, cursorPos.y + EDITOR_BIN_HEIGHT * i);
					drawList->AddText(labelMin, textCol, getChannelName(selectedObject->animationChannels[i]->type));
				}

				ImVec2 keyframeAreaSize = ImVec2(availX - EDITOR_LABEL_AREA_WIDTH,
				                                 EDITOR_BIN_HEIGHT * EDITOR_PROP_BIN_COUNT);

				drawList->AddLine(ImVec2(cursorPos.x + EDITOR_LABEL_AREA_WIDTH, cursorPos.y),
				                  ImVec2(cursorPos.x + EDITOR_LABEL_AREA_WIDTH, cursorPos.y + timelineHeight),
				                  borderCol);

				drawList->PushClipRect(ImVec2(cursorPos.x + EDITOR_LABEL_AREA_WIDTH, cursorPos.y),
				                       ImVec2(cursorPos.x + EDITOR_LABEL_AREA_WIDTH + keyframeAreaSize.x,
				                              cursorPos.y + keyframeAreaSize.y));

				// Draw the keyframes
				int id = 0;
				for (int i = 0; i < selectedObject->animationChannels.size(); i++)
				{
					AnimationChannel* channel = selectedObject->animationChannels[i];

					ImVec2 binMin = ImVec2(cursorPos.x, cursorPos.y + EDITOR_BIN_HEIGHT * i);
					ImGui::SetCursorScreenPos(binMin);

					for (int j = 0; j < channel->keyframes.size(); j++)
					{
						Keyframe kf = channel->keyframes[j];

						ImVec2 kfPos = ImVec2(
							EDITOR_KEYFRAME_OFFSET + binMin.x + (kf.time - startTime) / (endTime - startTime) *
							keyframeAreaSize.x,
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
					ImGui::InvisibleButton("##KeyframeBin", ImVec2(keyframeAreaSize.x, EDITOR_BIN_HEIGHT));

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						float kfTime = startTime + ((io.MousePos.x - btnMinX) / keyframeAreaSize.x) * (endTime -
							startTime);

						if (kfTime > 0.0f)
						{
							Keyframe kf = Keyframe();
							kf.time = kfTime;
							kf.value = 0.0f;

							channel->insertKeyframe(kf);
						}
					}

					ImGui::PopID();
				}

				drawList->PopClipRect();

				// Frames
				drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + timelineHeight),
				                  borderCol);

				drawList->PopClipRect();

				// Reset cursor
				ImGui::SetCursorScreenPos(cursorPos);
				ImGui::ItemSize(ImVec2(availX, timelineHeight));
			}

			// Draw keyframe editor
			ImGui::Separator();
			if (selectedKeyframe.has_value() && selectedChannel != nullptr)
			{
				Keyframe kf = selectedKeyframe.value();

				bool kfChanged = false;
				ImGui::DragFloat("Keyframe Time", &kf.time, 0.1f);
				kfChanged = kfChanged || ImGui::IsItemEdited();
				ImGui::DragFloat("Keyframe Value", &kf.value, 0.1f);
				kfChanged = kfChanged || ImGui::IsItemEdited();

				if (kfChanged)
				{
					std::vector<Keyframe>::iterator it = std::find(selectedChannel->keyframes.begin(),
					                                               selectedChannel->keyframes.end(),
					                                               selectedKeyframe.value());
					selectedChannel->keyframes.erase(it);

					selectedChannel->insertKeyframe(kf);
					selectedKeyframe = kf;

					levelManager->update(levelManager->time);
				}
			}
			else
			{
				ImGui::Text("No keyframe selected");
			}
		}
		else
		{
			ImGui::Text("No object selected");
		}
	}
	ImGui::End();
}

const char* Properties::getChannelName(AnimationChannelType channelType)
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
	}

	return "Unknown type";
}
