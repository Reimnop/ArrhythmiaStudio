#include "Properties.h"
#include "LevelManager.h"

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
				const int binCount = 5;
				const float binHeight = 20.0f;

				const float labelAreaWidth = 85.0f;
				const float kfSize = 12.0f;
				const float kfOffset = kfSize + labelAreaWidth;

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

				ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);

				// Draw channel labels
				for (int i = 0; i < selectedObject->animationChannels.size(); i++)
				{
					ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);

					ImVec2 labelMin = ImVec2(cursorPos.x + 2.0f, cursorPos.y + binHeight * i);
					drawList->AddText(labelMin, textCol, getChannelName(selectedObject->animationChannels[i]->type));
				}

				ImVec2 keyframeAreaSize = ImVec2(availRegion.x - labelAreaWidth, availRegion.y);

				drawList->AddLine(ImVec2(cursorPos.x + labelAreaWidth, cursorPos.y),
				                  ImVec2(cursorPos.x + labelAreaWidth, cursorPos.y + timelineHeight), borderCol);

				drawList->PushClipRect(ImVec2(cursorPos.x + labelAreaWidth, cursorPos.y),
				                       ImVec2(cursorPos.x + labelAreaWidth + keyframeAreaSize.x,
				                              cursorPos.y + keyframeAreaSize.y));

				// Draw the keyframes
				int id = 0;
				for (int i = 0; i < selectedObject->animationChannels.size(); i++)
				{
					AnimationChannel* channel = selectedObject->animationChannels[i];

					ImVec2 binMin = ImVec2(cursorPos.x, cursorPos.y + binHeight * i);

					for (int j = 0; j < channel->keyframes.size(); j++)
					{
						const ImU32 inactiveCol = ImGui::GetColorU32(ImVec4(0.729f, 0.729f, 0.729f, 1.000f));
						const ImU32 activeCol = ImGui::GetColorU32(ImVec4(0.384f, 0.384f, 0.384f, 1.000f));

						Keyframe kf = channel->keyframes[j];

						ImVec2 kfPos = ImVec2(
							kfOffset + binMin.x + (kf.time - startTime) / (endTime - startTime) * keyframeAreaSize.x,
							binMin.y + binHeight * 0.5f);

						id++;
						ImGui::PushID(id);

						ImVec2 btnMin = ImVec2(kfPos.x - kfSize * 0.5f, binMin.y);

						ImGui::SetCursorScreenPos(btnMin);
						if (ImGui::InvisibleButton("##Keyframe", ImVec2(kfSize, binHeight)))
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

						if (selectedKeyframe.has_value() && selectedKeyframe.value() == kf && selectedChannel == channel)
						{
							kfActive = true;
						}

						ImGui::PopID();

						drawList->AddQuadFilled(
							ImVec2(kfPos.x, kfPos.y - kfSize * 0.5f),
							ImVec2(kfPos.x - kfSize * 0.5f, kfPos.y),
							ImVec2(kfPos.x, kfPos.y + kfSize * 0.5f),
							ImVec2(kfPos.x + kfSize * 0.5f, kfPos.y),
							kfActive ? activeCol : inactiveCol);
					}

					ImGui::PushID(channel);

					ImGui::SetCursorScreenPos(binMin);
					ImGui::InvisibleButton("##KeyframeBin", ImVec2(keyframeAreaSize.x, binHeight));

					if (ImGui::IsItemHovered() && ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) && ImGui::IsKeyPressed(GLFW_KEY_A, false))
					{
						float kfTime = levelManager->time - selectedObject->startTime;

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
				drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availRegion.x, cursorPos.y + timelineHeight),
				                  borderCol);

				drawList->PopClipRect();

				// Reset cursor
				ImGui::SetCursorScreenPos(cursorPos);
				ImGui::ItemSize(ImVec2(availRegion.x, timelineHeight));
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
					std::vector<Keyframe>::iterator it = std::find(selectedChannel->keyframes.begin(), selectedChannel->keyframes.end(), selectedKeyframe.value());
					selectedChannel->keyframes.erase(it);

					selectedChannel->insertKeyframe(kf);
					selectedKeyframe = kf;
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
