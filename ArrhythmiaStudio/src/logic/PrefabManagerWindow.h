#pragma once

#include "Prefab.h"

#include <optional>

class PrefabManagerWindow
{
public:
    static inline PrefabManagerWindow* inst;

    bool isOpen = false;

    PrefabManagerWindow();

    void newPrefabPopup();
    void openNewPrefabPopup();
private:
    std::optional<Prefab*> selectedPrefab;

    std::string tempPrefabName;

    void onLayout();
};
