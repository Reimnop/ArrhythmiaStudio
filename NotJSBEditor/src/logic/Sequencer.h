#pragma once

#include <optional>
#include <algorithm>
#include <imgui/imgui.h>

#include "../rendering/ImGuiController.h"

class LevelManager;
class Sequencer {
public:
    Sequencer(LevelManager* levelManager);
private:
    struct StripDragData {
        int currentStrip;
        float currentBin;
        bool verticalDrag;
    };

	LevelManager* levelManager;

    std::optional<StripDragData> dragData;

    float startTime;
    float endTime;
    
    void onLayout();
    bool intersectPointRect(ImVec2 min, ImVec2 max, ImVec2 pos);
};