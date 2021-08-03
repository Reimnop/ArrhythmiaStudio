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
    for (int i = 0; i < 10; i++)
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

    sequencer = new Sequencer(this);

    updateAllObjectActions();
}

void LevelManager::update(float time) {
    if (time >= lastTime) 
    { 
        while (actionIndex < objectActions.size() && objectActions[actionIndex].time <= time) {
            switch (objectActions[actionIndex].type) {
            case ObjectActionType_Spawn:
                objectActions[actionIndex].levelObject->node->active = true;
                break;
            case ObjectActionType_Kill:
                objectActions[actionIndex].levelObject->node->active = false;
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
                objectActions[actionIndex].levelObject->node->active = false;
                break;
            case ObjectActionType_Kill:
                objectActions[actionIndex].levelObject->node->active = true;
                break;
            }
            actionIndex--;
        }
    }

    lastTime = time;
}

void LevelManager::updateAllObjectActions() {
    // Clear and re-add object actions
    objectActions.clear();

    for (int i = 0; i < levelObjects.size(); i++) {
        const float EPSILON = 0.0001f;

        ObjectAction spawnAction = ObjectAction();
        spawnAction.time = levelObjects[i]->startTime;
        spawnAction.type = ObjectActionType_Spawn;
        spawnAction.levelObject = levelObjects[i];

        ObjectAction killAction = ObjectAction();
        killAction.time = levelObjects[i]->killTime + EPSILON;
        killAction.type = ObjectActionType_Kill;
        killAction.levelObject = levelObjects[i];

        insertAction(spawnAction);
        insertAction(killAction);
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
    node->active = false;
    
    MeshRenderer* renderer = new MeshRenderer();
    renderer->mesh = mesh;
    renderer->material = material;

    node->renderer = renderer;

    levelObject->node = node;
}