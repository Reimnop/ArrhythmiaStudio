#include "LevelObject.h"

#include <utils.h>

#include "LevelManager.h"
#include "../rendering/MeshRenderer.h"
#include "ShapeManager.h"

LevelObject::LevelObject()
{
    node = nullptr;
    id = Utils::randomId();
}

LevelObject::LevelObject(nlohmann::json j)
{
    name = j["name"].get<std::string>();
    id = j["id"].get<uint64_t>();
    parentId = j["parent"].get<uint64_t>();
    startTime = j["start"].get<float>();
    killTime = j["kill"].get<float>();
    depth = j["depth"].get<float>();
    isText = j.contains("is_text") && j["is_text"].get<bool>();
    if (isText) 
    {
        text = j["text"].get<std::string>();
    }
    else
    {
        shapeIndex = j["shape"].get<int>();
    }
    colorSlotIndex = j["color_slot"].get<int>();
    editorBinIndex = j["editor_bin"].get<int>();

    if (j.contains("layer"))
    {
        layer = j["layer"].get<int>();
    }

    nlohmann::json::array_t arr = j["channels"].get<nlohmann::json::array_t>();
    for (int i = 0; i < arr.size(); i++)
    {
        insertChannel(new AnimationChannel(arr[i]));
    }

    node = nullptr;
}

LevelObject::~LevelObject()
{
    Level* level = LevelManager::inst->level;

    // Remove from parent
    if (parentId)
    {
        LevelObject* parent = level->levelObjects[parentId];
        parent->childrenId.erase(id);
    }

    // Cache children ids
    const size_t childrenCount = childrenId.size();
    uint64_t* children = new uint64_t[childrenCount];
    std::copy(childrenId.begin(), childrenId.end(), children);

    // Unparent all children
    for (int i = 0; i < childrenCount; i++)
    {
        level->levelObjects[children[i]]->setParent(nullptr);
    }

    delete[] children;

    for (const AnimationChannel* channel : animationChannels)
    {
        delete channel;
    }

    delete node;
}

LevelObjectProperties LevelObject::dumpProperties() const
{
    LevelObjectProperties properties = LevelObjectProperties();
    properties.startTime = startTime;
    properties.killTime = killTime;
    properties.isText = isText;
    properties.text = text;
    properties.shapeIndex = shapeIndex;
    properties.editorBinIndex = editorBinIndex;
    properties.colorSlotIndex = colorSlotIndex;
    properties.depth = depth;
    properties.layer = layer;

    return properties;
}

void LevelObject::applyProperties(LevelObjectProperties properties)
{
    LevelManager* levelManager = LevelManager::inst;
    const Level* level = levelManager->level;

    // This check is not necessary but it's good to have it there because re-insertion is expensive
    if (properties.startTime != startTime || properties.killTime != killTime)
    {
        startTime = properties.startTime;
        killTime = properties.killTime;

        levelManager->recalculateObjectAction(this);
        levelManager->recalculateActionIndex(levelManager->time);
    }

    isText = properties.isText;
    text = properties.text;
    shapeIndex = properties.shapeIndex;
    editorBinIndex = properties.editorBinIndex;
    colorSlotIndex = properties.colorSlotIndex;
    depth = properties.depth;
    layer = properties.layer;

    if (node)
    {
        delete node->renderer;

        if (isText)
        {
            TextRenderer* renderer = new TextRenderer(LevelManager::inst->mainFont);
            renderer->material = level->colorSlots[colorSlotIndex]->material;
            renderer->setText(text);

            node->renderer = renderer;
        }
        else
        {
            MeshRenderer* renderer = new MeshRenderer();
            renderer->mesh = ShapeManager::inst->shapes[shapeIndex].mesh;
            renderer->material = level->colorSlots[colorSlotIndex]->material;
            renderer->shader = LevelManager::inst->unlitShader;

            node->renderer = renderer;
        }

        levelManager->updateObject(this);
    }
}

void LevelObject::setParent(LevelObject* newParent)
{
    Level* level = LevelManager::inst->level;

    // Remove from old parent
    if (parentId)
    {
        level->levelObjects[parentId]->childrenId.erase(id);
    }

    // Add to new parent
    if (newParent)
    {
        newParent->childrenId.emplace(id);
        parentId = newParent->id;
        node->setParent(newParent->node);
    }
    else
    {
        parentId = 0;
        node->setParent(nullptr);
    }
}

void LevelObject::genActionPair(ObjectAction* spawnAction, ObjectAction* killAction)
{
    ObjectAction spawn = ObjectAction();
    spawn.time = startTime;
    spawn.type = ObjectActionType_Spawn;
    spawn.levelObject = this;

    ObjectAction kill = ObjectAction();
    kill.time = killTime + 0.0001f;
    kill.type = ObjectActionType_Kill;
    kill.levelObject = this;

    *spawnAction = spawn;
    *killAction = kill;
}

void LevelObject::insertChannel(AnimationChannel* value)
{
    if (hasChannel(value->type))
    {
        return;
    }

    const std::vector<AnimationChannel*>::iterator it = std::lower_bound(animationChannels.begin(), animationChannels.end(),
                                                                        value,
                                                                        [](const AnimationChannel* a, const AnimationChannel* b)
                                                                        {
                                                                            return a->type < b->type;
                                                                        });
    animationChannels.insert(it, value);

    animationChannelLookup[value->type] = true;
}

void LevelObject::eraseChannel(AnimationChannelType type)
{
    const std::vector<AnimationChannel*>::iterator it = std::find_if(animationChannels.begin(), animationChannels.end(),
                                                                     [type](const AnimationChannel* a)
                                                                     {
                                                                         return a->type == type;
                                                                     });
    delete (*it);
    animationChannels.erase(it);

    animationChannelLookup[type] = false;
}

AnimationChannel* LevelObject::getChannel(AnimationChannelType type)
{
    const std::vector<AnimationChannel*>::iterator it = std::find_if(animationChannels.begin(), animationChannels.end(),
                                                                     [type](const AnimationChannel* a)
                                                                     {
                                                                         return a->type == type;
                                                                     });

    return *it;
}

bool LevelObject::hasChannel(AnimationChannelType type)
{
    return animationChannelLookup[type];
}

nlohmann::ordered_json LevelObject::toJson()
{
    nlohmann::ordered_json j;
    j["name"] = name;
    j["id"] = id;
    j["parent"] = parentId;
    j["start"] = startTime;
    j["kill"] = killTime;
    j["depth"] = depth;
    j["is_text"] = isText;
    if (isText)
    {
        j["text"] = text;
    }
    else
    {
        j["shape"] = shapeIndex;
    }
    j["color_slot"] = colorSlotIndex;
    j["editor_bin"] = editorBinIndex;
    j["layer"] = layer;

    j["channels"] = nlohmann::json::array();
    for (int i = 0; i < animationChannels.size(); i++)
    {
        j["channels"][i] = animationChannels[i]->toJson();
    }

    return j;
}
