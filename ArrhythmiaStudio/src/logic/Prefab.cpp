#include "Prefab.h"
#include "utils.h"

Prefab::Prefab(std::string& name, std::unordered_set<LevelObject*>& objects)
{
    this->name = name;
    id = Utils::randomId();

    float minTime = INFINITY;
    for (LevelObject* obj : objects)
    {
        minTime = std::min(minTime, obj->startTime);
    }

    for (LevelObject* obj : objects)
    {
        this->objects.push_back(obj->toJson(minTime));
    }
}

Prefab::Prefab(const json& j)
{
    name = j["name"].get<std::string>();
    id = j["id"].get<uint64_t>();
    for (const json& obj : j["objects"]) {
        objects.push_back(obj);
    }
}

json Prefab::toJson()
{
    json j;
    j["name"] = name;
    j["id"] = id;
    j["objects"] = objects;
    return j;
}

json::array_t Prefab::getObjectsJson()
{
    return objects;
}