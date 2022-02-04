#pragma once
#include "AnimateableObjectBehaviour.h"
#include "../Prefab.h"

class PrefabInstanceObjectBehaviour : public AnimateableObjectBehaviour
{
public:
    PrefabInstanceObjectBehaviour(LevelObject* levelObject);
    ~PrefabInstanceObjectBehaviour() override = default;

    static LevelObjectBehaviour* create(LevelObject* object);

    void update(float time) override;

    void readJson(json& j) override;
    void writeJson(json& j) override;

    void drawEditor() override;
private:
    void changePrefab(Prefab* prefab);

    uint64_t prefabId = 0;
    std::optional<std::unique_ptr<ObjectSpawner>> spawner;
};
