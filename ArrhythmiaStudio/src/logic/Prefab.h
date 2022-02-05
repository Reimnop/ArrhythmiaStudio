#pragma once
#include <unordered_set>

#include "json.hpp"
#include "LevelObject.h"

using namespace nlohmann;

class Prefab {
public:
    std::string name;
    uint64_t id;

    // This constructor makes a new prefab
    Prefab(std::string& name, std::unordered_set<LevelObject*>& objects);
    // This constructor loads a prefab from json
    Prefab(const json& j);

    json toJson();
    json::array_t getObjectsJson();
private:
    json::array_t objects;
};
