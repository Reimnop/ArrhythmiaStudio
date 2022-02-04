#pragma once

#include "Prefab.h"

#include <optional>

class PrefabManagerWindow
{
public:
    bool isOpen = false;

    PrefabManagerWindow();
private:
    std::optional<Prefab*> selectedPrefab;

    void onLayout();
};
