#include "Properties.h"
#include "LevelManager.h"

Properties::Properties(LevelManager* levelManager) {
	this->levelManager = levelManager;

    startTime = 0.0f;
    endTime = 10.0f;

	ImGuiController::onLayout.push_back(std::bind(&Properties::onLayout, this));
}

void Properties::onLayout() {
	// Open Dope Sheet window
	if (ImGui::Begin("Properties")) {
        if (levelManager->selectedObjectIndex != -1) {
            LevelObject* selectedObject = levelManager->levelObjects[levelManager->selectedObjectIndex];

            char* nameBuffer = (char*)selectedObject->name.c_str();
            ImGui::InputText("Object Name", nameBuffer, 256);

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

                    selectedObject->animationChannels.push_back(channel);
                }

                if (!selectedObject->hasChannel(AnimationChannelType_PositionY) && ImGui::Selectable("Position Y"))
                {
                    Keyframe first = Keyframe();
                    first.time = 0.0f;
                    first.value = 0.0f;

                    AnimationChannel* channel = new AnimationChannel(AnimationChannelType_PositionY, 1, &first);

                    selectedObject->animationChannels.push_back(channel);
                }

                if (!selectedObject->hasChannel(AnimationChannelType_ScaleX) && ImGui::Selectable("Scale X"))
                {
                    Keyframe first = Keyframe();
                    first.time = 0.0f;
                    first.value = 1.0f;

                    AnimationChannel* channel = new AnimationChannel(AnimationChannelType_ScaleX, 1, &first);

                    selectedObject->animationChannels.push_back(channel);
                }

                if (!selectedObject->hasChannel(AnimationChannelType_ScaleY) && ImGui::Selectable("Scale Y"))
                {
                    Keyframe first = Keyframe();
                    first.time = 0.0f;
                    first.value = 1.0f;

                    AnimationChannel* channel = new AnimationChannel(AnimationChannelType_ScaleY, 1, &first);

                    selectedObject->animationChannels.push_back(channel);
                }

                if (!selectedObject->hasChannel(AnimationChannelType_Rotation) && ImGui::Selectable("Rotation"))
                {
                    Keyframe first = Keyframe();
                    first.time = 0.0f;
                    first.value = 0.0f;

                    AnimationChannel* channel = new AnimationChannel(AnimationChannelType_Rotation, 1, &first);

                    selectedObject->animationChannels.push_back(channel);
                }

                ImGui::EndPopup();
            }

            // Draw keyframe editor
            {
                const int binCount = 5;
                const float binHeight = 20.0f;

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

                const float labelAreaWidth = 85.0f;
                const float kfSize = binHeight / 2.0f;
                const float kfOffset = kfSize + labelAreaWidth;

                // Draw channel labels
                for (int i = 0; i < selectedObject->animationChannels.size(); i++) {
                    ImVec2 labelMin = ImVec2(cursorPos.x, cursorPos.y + binHeight * i);
                    ImGui::Text(getChannelName(selectedObject->animationChannels[i]->type));
                }

                ImVec2 keyframeAreaSize = ImVec2(availRegion.x - labelAreaWidth, availRegion.y);

                drawList->AddLine(ImVec2(cursorPos.x + labelAreaWidth, cursorPos.y), ImVec2(cursorPos.x + labelAreaWidth, cursorPos.y + timelineHeight), borderCol);

                drawList->PushClipRect(ImVec2(cursorPos.x + labelAreaWidth, cursorPos.y), ImVec2(cursorPos.x + labelAreaWidth + keyframeAreaSize.x, cursorPos.y + keyframeAreaSize.y));

                // Draw the keyframes
                for (int i = 0; i < selectedObject->animationChannels.size(); i++) {
                    AnimationChannel* channel = selectedObject->animationChannels[i];

                    ImVec2 binMin = ImVec2(cursorPos.x, cursorPos.y + binHeight * i);

                    for (Keyframe kf : channel->keyframes) {
                        ImVec2 kfPos = ImVec2(kfOffset + binMin.x + (kf.time - startTime) / (endTime - startTime) * keyframeAreaSize.x, binMin.y + (binHeight - kfSize) / 2.0f);

                        drawList->AddQuadFilled(
                            kfPos,
                            ImVec2(kfPos.x - kfSize / 2.0f, kfPos.y + kfSize / 2.0f),
                            ImVec2(kfPos.x, kfPos.y + kfSize),
                            ImVec2(kfPos.x + kfSize / 2.0f, kfPos.y + kfSize / 2.0f),
                            ImGui::GetColorU32(ImVec4(0.729f, 0.729f, 0.729f, 1.000f)));
                    }
                }

                drawList->PopClipRect();

                // Frames
                drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availRegion.x, cursorPos.y + timelineHeight), borderCol);

                drawList->PopClipRect();
            }
        }
        else 
        {
            ImGui::Text("No object selected");
        }

		ImGui::End();
	}
}

const char* Properties::getChannelName(AnimationChannelType channelType) {
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