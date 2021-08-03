#include "LevelManager.h"

#include <string>

glm::vec3 vertices[] = {
        glm::vec3(0.5f, 0.5f, 0.0f),
        glm::vec3(-0.5f, 0.5f, 0.0f),
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f)
};

uint32_t indices[] = {
    0, 1, 2,
    0, 2, 3
};

Mesh* mesh;
Shader* shader;
Material* material;

float randomFloat() {
    return std::rand() / (float)RAND_MAX;
}

LevelManager::LevelManager() {
    mesh = new Mesh(4, vertices, 6, indices);
    shader = new Shader("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
    material = new Material(shader, 0, nullptr);

	std::srand(8);

	// Add random objects
    for (int i = 0; i < 0; i++)
    {
        float start = randomFloat() * 60.0f;
        float end = start + 5.0f + randomFloat() * 20.0f;

        LevelObject* obj = new LevelObject("Object Index " + std::to_string(i));
        obj->startTime = start;
        obj->killTime = end;
        obj->editorBinIndex = (int)(randomFloat() * 10.0f);

        spawnNode(obj);

        levelObjects.push_back(obj);
    }

    for (LevelObject* obj : levelObjects) {
        for (int i = 0; i < 6; i++) {
            AnimationChannel* channel = new AnimationChannel((AnimationChannelType)i, 0, nullptr);

            for (int i = 0; i < 10; i++) {
                Keyframe kf = Keyframe();
                kf.time = i;
                kf.value = channel->type != AnimationChannelType_Rotation ? (randomFloat() * 2.0f - 1.0f) * 8.0f : (randomFloat() * 2.0f - 1.0f) * 90.0f;

                channel->insertKeyframe(kf);
            }

            obj->animationChannels.push_back(channel);
        }
    }

    timeline = new Timeline(this);
    dopeSheet = new DopeSheet(this);

    updateAllObjectActions();
}

void LevelManager::update(float time) {
    if (time >= lastTime) 
    { 
        while (actionIndex < objectActions.size() && objectActions[actionIndex].time <= time) {
            switch (objectActions[actionIndex].type) {
            case ObjectActionType_Spawn:
                objectActions[actionIndex].levelObject->node->setActive(true);
                aliveObjects.insert(objectActions[actionIndex].levelObject);
                break;
            case ObjectActionType_Kill:
                objectActions[actionIndex].levelObject->node->setActive(false);
                aliveObjects.erase(objectActions[actionIndex].levelObject);
                break;
            }
            actionIndex++;
        }
    }
    else
    {
        while (actionIndex >= 0 && objectActions[actionIndex].time > time) {
            switch (objectActions[actionIndex].type) {
            case ObjectActionType_Spawn:
                objectActions[actionIndex].levelObject->node->setActive(false);
                aliveObjects.erase(objectActions[actionIndex].levelObject);
                break;
            case ObjectActionType_Kill:
                objectActions[actionIndex].levelObject->node->setActive(true);
                aliveObjects.insert(objectActions[actionIndex].levelObject);
                break;
            }
            actionIndex--;
        }
    }

    lastTime = time;

    // Animate alive objects
    for (LevelObject* levelObject : aliveObjects) {
        for (AnimationChannel* channel : levelObject->animationChannels) {
            switch (channel->type) {
            case AnimationChannelType_PositionX:
                levelObject->node->transform->position.x = channel->update(time - levelObject->startTime);
                break;
            case AnimationChannelType_PositionY:
                levelObject->node->transform->position.y = channel->update(time - levelObject->startTime);
                break;
            case AnimationChannelType_ScaleX:
                levelObject->node->transform->scale.x = channel->update(time - levelObject->startTime);
                break;
            case AnimationChannelType_ScaleY:
                levelObject->node->transform->scale.y = channel->update(time - levelObject->startTime);
                break;
            case AnimationChannelType_Rotation:
                levelObject->node->transform->rotation = glm::quat(glm::vec3(0.0f, 0.0f, channel->update(time - levelObject->startTime) / 180.0f * 3.14159265359f));
                break;
            }
        }
    }
}

void LevelManager::updateAllObjectActions()
{
    // Clear and re-add object actions
    objectActions.clear();

    for (int i = 0; i < levelObjects.size(); i++) {
        ObjectAction spawnAction, killAction;
        levelObjects[i]->genActionPair(&spawnAction, &killAction);

        insertAction(spawnAction);
        insertAction(killAction);
    }
}

void LevelManager::recalculateActionIndex(float time) {
    // Reset all objects
    for (LevelObject* levelObject : levelObjects) {
        levelObject->node->setActive(false);
    }

    // Reset action index and recalculate
    actionIndex = 0;
    while (actionIndex < objectActions.size() && objectActions[actionIndex].time <= time) {
        switch (objectActions[actionIndex].type) {
        case ObjectActionType_Spawn:
            objectActions[actionIndex].levelObject->node->setActive(true);
            aliveObjects.insert(objectActions[actionIndex].levelObject);
            break;
        case ObjectActionType_Kill:
            objectActions[actionIndex].levelObject->node->setActive(false);
            aliveObjects.erase(objectActions[actionIndex].levelObject);
            break;
        }
        actionIndex++;
    }
}

bool actionComp(ObjectAction a, ObjectAction b) {
    return a.time < b.time;
}

void LevelManager::insertAction(ObjectAction value) {
    if (objectActions.size() == 0) {
        objectActions.push_back(value);
        return;
    }

    std::vector<ObjectAction>::iterator it = std::lower_bound(objectActions.begin(), objectActions.end(), value, actionComp);
    objectActions.insert(it, value);
}

void LevelManager::spawnNode(LevelObject* levelObject) {
    SceneNode* node = new SceneNode(levelObject->name);
    node->setActive(false);
    
    MeshRenderer* renderer = new MeshRenderer();
    renderer->mesh = mesh;
    renderer->material = material;

    node->renderer = renderer;

    levelObject->node = node;
}