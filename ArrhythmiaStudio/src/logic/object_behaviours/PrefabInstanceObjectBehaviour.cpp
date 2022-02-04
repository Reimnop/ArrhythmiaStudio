#include "PrefabInstanceObjectBehaviour.h"
#include "../Level.h"
#include "imgui/imgui.h"

PrefabInstanceObjectBehaviour::PrefabInstanceObjectBehaviour(LevelObject* levelObject) : AnimateableObjectBehaviour(levelObject)
{

}

LevelObjectBehaviour* PrefabInstanceObjectBehaviour::create(LevelObject* object)
{
    return new PrefabInstanceObjectBehaviour(object);
}

void PrefabInstanceObjectBehaviour::update(float time)
{
    if (spawner.has_value())
    {
        spawner.value()->update(time - baseObject->startTime);
    }

    AnimateableObjectBehaviour::update(time);
}

void PrefabInstanceObjectBehaviour::drawEditor()
{
    Level& level = *Level::inst;
    if (ImGui::BeginCombo("Prefab", prefabId ? level.prefabs[prefabId]->name.c_str() : "None"))
    {
        for (auto &[id, prefab] : level.prefabs)
        {
            if (ImGui::Selectable(prefab->name.c_str(), id == prefabId))
            {
                changePrefab(prefab);
            }
        }

        ImGui::EndCombo();
    }
    AnimateableObjectBehaviour::drawEditor();
}

void PrefabInstanceObjectBehaviour::readJson(json& j)
{
    AnimateableObjectBehaviour::readJson(j);
    prefabId = j["prefab"].get<uint64_t>();
    changePrefab(Level::inst->prefabs[prefabId]);
}

void PrefabInstanceObjectBehaviour::writeJson(json& j)
{
    j["prefab"] = prefabId;
    AnimateableObjectBehaviour::writeJson(j);
}

void PrefabInstanceObjectBehaviour::changePrefab(Prefab* prefab)
{
    json::array_t objects = prefab->getObjectsJson();
    spawner = std::make_unique<ObjectSpawner>(objects, baseObject->node);
    prefabId = prefab->id;
}