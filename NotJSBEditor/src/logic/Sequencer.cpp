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
            ImU32 binCol = ImGui::GetColorU32(ImVec4(0.180f, 0.180f, 0.180f, 1.000f));
            if (i % 2 == 0)
            {
                binCol = ImGui::GetColorU32(ImVec4(0.120f, 0.120f, 0.120f, 1.000f));
            }

            drawList->AddRectFilled(
                ImVec2(cursorPos.x, cursorPos.y + i * binHeight),
                ImVec2(cursorPos.x + availRegion.x, cursorPos.y + (i + 1) * binHeight),
                binCol);
        }

        // Draw editor strips
        ImU32 textCol = ImGui::GetColorU32(ImVec4(0.120f, 0.120f, 0.120f, 1.000f));

        for (int i = 0; i < levelManager->levelObjects.size(); i++)
        {
            LevelObject* levelObject = &levelManager->levelObjects[i];

            // Calculate start and end position in pixel
            float startPos = (levelObject->startTime - startTime) / (endTime - startTime) * availRegion.x;
            float endPos = (levelObject->endTime - startTime) / (endTime - startTime) * availRegion.x;

            ImVec2 stripMin = ImVec2(cursorPos.x + startPos, cursorPos.y + levelObject->editorBinIndex * binHeight);
            ImVec2 stripMax = ImVec2(cursorPos.x + endPos, cursorPos.y + (levelObject->editorBinIndex + 1) * binHeight);

            ImU32 stripCol = ImGui::GetColorU32(ImVec4(0.729f, 0.729f, 0.729f, 1.000f));

            if (intersectPointRect(stripMin, stripMax, io.MousePos))
            {
                stripCol = ImGui::GetColorU32(ImVec4(0.384f, 0.384f, 0.384f, 1.000f));
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && intersectPointRect(stripMin, stripMax, io.MouseClickedPos[(int)ImGuiMouseButton_Left]))
            {
                StripDragData newDragData;

                newDragData.currentStrip = i;
                newDragData.currentBin = levelObject->editorBinIndex;
                newDragData.verticalDrag = ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT);

                dragData = newDragData;
            }

            if (dragData.has_value() && dragData.value().currentStrip == i)
            {
                if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    dragData.reset();
                }
                else
                {
                    stripCol = ImGui::GetColorU32(ImVec4(0.384f, 0.384f, 0.384f, 1.000f));

                    if (!dragData.value().verticalDrag)
                    {
                        // Horizontal drag
                        float timeDelta = (io.MouseDelta.x / availRegion.x) * (endTime - startTime);
                        levelObject->startTime += timeDelta;
                        levelObject->endTime += timeDelta;
                    }
                    else
                    {
                        // Vertical drag
                        dragData.value().currentBin += io.MouseDelta.y / binHeight;
                        levelObject->editorBinIndex = (int)dragData.value().currentBin;
                    }
                }
            }

            // Draw the strip a bit shorter than the bin
            drawList->AddRectFilled(stripMin, stripMax, stripCol);
            drawList->AddText(ImVec2(cursorPos.x + startPos + 1.0f, cursorPos.y + levelObject->editorBinIndex * binHeight), textCol, levelObject->name.c_str());
        }

        drawList->PopClipRect();

        // Frames
        ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availRegion.x, cursorPos.y + clippedSequencerHeight), borderCol);

        ImGui::End();
    }
}

bool Sequencer::intersectPointRect(ImVec2 min, ImVec2 max, ImVec2 pos) {
    if (pos.x > min.x && pos.y > min.y &&
        pos.x < max.x && pos.y < max.y)
        return true;

    return false;
}