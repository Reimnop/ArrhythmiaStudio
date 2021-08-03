#pragma once

#include <functional>

#include "../rendering/ImGuiController.h"

class LevelManager;
class DopeSheet {
public:
    DopeSheet(LevelManager* levelManager);
private:
    LevelManager* levelManager;

    void onLayout();
};