#include "ObjectSpawner.h"
#include "LevelObject.h"

ObjectSpawner::ObjectSpawner(json::array_t& j)
{
	aliveObjects.clear();
	for (json& objJ : j)
	{
		LevelObject* obj = new LevelObject(objJ, this);
		insertObject(obj);
		insertActivateList(obj);
		insertDeactivateList(obj);
	}
	recalculateObjectsState();

	for (auto& [id, obj] : levelObjects) 
	{
		obj->initializeParent();
	}
}

ObjectSpawner::~ObjectSpawner()
{
	for (auto& [id, obj] : levelObjects)
	{
		delete obj;
	}
}

void ObjectSpawner::update(float time)
{
	this->currentTime = time;

	if (time > lastTime)
	{
		updateForward();
	}
	else if (time < lastTime)
	{
		updateReverse();
	}

	// Update objects
	for (LevelObject* obj : aliveObjects)
	{
		obj->update(time);
	}

	lastTime = time;
}

void ObjectSpawner::addObject(LevelObject* object)
{
	insertObject(object);
	insertActivateList(object);
	insertDeactivateList(object);
	recalculateObjectsState();
}

void ObjectSpawner::deleteObject(LevelObject* object)
{
	removeObject(object);
	removeActivateList(object);
	removeDeactivateList(object);
	recalculateObjectsState();

	delete object;
}

void ObjectSpawner::insertObject(LevelObject* object)
{
	object->spawner = this;
	levelObjects.emplace(object->id, object);
}

void ObjectSpawner::removeObject(LevelObject* object)
{
	object->spawner = nullptr;
	levelObjects.erase(object->id);
}

void ObjectSpawner::insertActivateList(LevelObject* object)
{
	activateList.insert(
		std::upper_bound(activateList.begin(), activateList.end(), object,
			[](LevelObject* a, LevelObject* b)
			{
				return a->startTime < b->startTime;
			}), object);
}

void ObjectSpawner::insertDeactivateList(LevelObject* object)
{
	deactivateList.insert(
		std::upper_bound(deactivateList.begin(), deactivateList.end(), object,
			[](LevelObject* a, LevelObject* b)
			{
				return a->endTime < b->endTime;
			}), object);
}

void ObjectSpawner::removeActivateList(LevelObject* object)
{
	activateList.erase(std::remove(activateList.begin(), activateList.end(), object));
}

void ObjectSpawner::removeDeactivateList(LevelObject* object)
{
	deactivateList.erase(std::remove(deactivateList.begin(), deactivateList.end(), object));
}

void ObjectSpawner::recalculateObjectsState()
{
	std::unordered_map<LevelObject*, int> activeTable;
	aliveObjects.clear();
	activateIndex = 0;
	deactivateIndex = 0;

	for (auto& [id, obj] : levelObjects)
	{
		activeTable[obj] = 0;
	}

	for (LevelObject* object : activateList)
	{
		if (object->startTime > currentTime)
		{
			break;
		}

		activateIndex++;
		activeTable[object]++;
	}

	for (LevelObject* object : deactivateList)
	{
		if (object->endTime > currentTime)
		{
			break;
		}

		deactivateIndex++;
		activeTable[object]--;
	}

	for (auto& [obj, activeIndex] : activeTable)
	{
		if (activeIndex % 2)
		{
			obj->node->setActive(true);
			aliveObjects.insert(obj);
		}
		else
		{
			obj->node->setActive(false);
		}
	}
}

void ObjectSpawner::updateForward()
{
	while (activateIndex < activateList.size() && currentTime >= activateList[activateIndex]->startTime)
	{
		LevelObject* obj = activateList[activateIndex];
		obj->node->setActive(true);
		aliveObjects.insert(obj);
		activateIndex++;
	}

	while (deactivateIndex < deactivateList.size() && currentTime >= deactivateList[deactivateIndex]->endTime)
	{
		LevelObject* obj = deactivateList[deactivateIndex];
		obj->node->setActive(false);
		aliveObjects.erase(obj);
		deactivateIndex++;
	}
}

void ObjectSpawner::updateReverse()
{
	while (deactivateIndex - 1 >= 0 && currentTime < deactivateList[deactivateIndex - 1]->endTime)
	{
		LevelObject* obj = deactivateList[deactivateIndex - 1];
		obj->node->setActive(true);
		aliveObjects.insert(obj);
		deactivateIndex--;
	}

	while (activateIndex - 1 >= 0 && currentTime < activateList[activateIndex - 1]->startTime)
	{
		LevelObject* obj = activateList[activateIndex - 1];
		obj->node->setActive(false);
		aliveObjects.erase(obj);
		activateIndex--;
	}
}

json::array_t ObjectSpawner::toJson()
{
	json::array_t arr;
	for (auto &[id, obj] : levelObjects)
	{
		arr.push_back(obj->toJson());
	}
	return arr;
}
