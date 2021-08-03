#include "Sequencer.h"
#include "LevelManager.h"

Sequencer::Sequencer(LevelManager* levelManager) {
	this->levelManager = levelManager;

	startTime = 0.0f;
	endTime = 80.0f;

	ImGuiController::onLayout.push_back(std::bind(&Sequencer::onLayout, this));
}

void Sequencer::onLayout() {
    // Open a sequence window
    if (ImGui::Begin("Sequencer"))
    {
        const int binCount = 10;
        const float binHeight = 20.0f;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 availRegion = ImGui::GetContentRegionAvail();

        float sequencerHeight = binCount * binHeight;
        float clippedSequencerHeight = std::min(availRegion.y, sequencerHeight);

        ImVec2 clipSize = ImVec2(availRegion.x, clippedSequencerHeight);

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

        // Draw editor strips
        ImU32 textCol = ImGui::GetColorU32(ImVec4(0.120f, 0.120f, 0.120f, 1.000f));

        bool atLeastOneStripClicked = false;
        for (int i = 0; i < levelManager->levelObjects.size(); i++)
        {
            const ImU32 inactiveCol = ImGui::GetColorU32(ImVec4(0.729f, 0.729f, 0.729f, 1.000f));
            const ImU32 activeCol = ImGui::GetColorU32(ImVec4(0.384f, 0.384f, 0.384f, 1.000f));

            LevelObject* levelObject = levelManager->levelObjects[i];

            if (levelObject->killTime < startTime || levelObject->startTime > endTime) 
            {
                continue;
            }

            // Calculate start and end position in pixel
            float startPos = (levelObject->startTime - startTime) / (endTime - startTime) * availRegion.x;
            float endPos = (levelObject->killTime - startTime) / (endTime - startTime) * availRegion.x;

            // Calculate strip params
            ImVec2 stripMin = ImVec2(cursorPos.x + startPos, cursorPos.y + levelObject->editorBinIndex * binHeight);
            ImVec2 stripMax = ImVec2(cursorPos.x + endPos, cursorPos.y + (levelObject->editorBinIndex + 1) * binHeight);

            ImVec2 stripSize = ImVec2(stripMax.x - stripMin.x, stripMax.y - stripMin.y);

            int id = i + 1;

            ImGui::PushID(id);

            ImGui::SetCursorScreenPos(stripMin);
            if (ImGui::InvisibleButton("##Strip", stripSize)) 
            {
                atLeastOneStripClicked = true;
                selectedObjectIndex = i;
            }

            bool stripActive = false;

            if (ImGui::IsItemHovered())
            {
                stripActive = true;
            }

            if (ImGui::IsItemActive())
            {
                stripActive = true;

                // Dragging
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    ImVec2 delta = io.MouseDelta;
                    float timeDelta = (delta.x / availRegion.x) * (endTime - startTime);

                    timeDelta = std::clamp(timeDelta, -levelObject->startTime, endTime - levelObject->killTime);

                    levelObject->startTime += timeDelta;
                    levelObject->killTime += timeDelta;
                }
            }

            if (selectedObjectIndex == i) 
            {
                stripActive = true;

                // Changing editor bins
                if (ImGui::IsWindowFocused()) 
                {
                    levelObject->editorBinIndex -= io.MouseWheel;
                    levelObject->editorBinIndex = std::clamp(levelObject->editorBinIndex, 0, binCount - 1);
                }
            }

            ImGui::PopID();

            // Draw the strip
            drawList->AddRectFilled(stripMin, stripMax, activeCol, 5.0f);
            drawList->AddRectFilled(ImVec2(stripMin.x + 1.0f, stripMin.y + 1.0f), ImVec2(stripMax.x - 1.0f, stripMax.y - 1.0f), stripActive ? activeCol : inactiveCol, 5.0f);
            drawList->AddText(ImVec2(cursorPos.x + startPos + 2.5f, cursorPos.y + levelObject->editorBinIndex * binHeight), textCol, levelObject->name.c_str());
        }
        
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !atLeastOneStripClicked) 
        {
            selectedObjectIndex = -1;
        }

        drawList->PopClipRect();

        // Frames
        ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availRegion.x, cursorPos.y + clippedSequencerHeight), borderCol);

        ImGui::End();
    }
}