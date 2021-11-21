#include "Level.h"
#include "object_behaviours/LevelObjectBehaviour.h"

Level::Level() 
{
	
}

Level::~Level()
{
	
}

void Level::update(float t)
{
	time = t;

	if (time > lastTime)
	{
		updateForward();
	}
	else if (time < lastTime)
	{
		updateReverse();
	}

	for (LevelObject* obj : aliveObjects)
	{
		obj->behaviour->update(time);
	}

	lastTime = time;
}

void Level::addObject(LevelObject* object)
{
	insertObject(object);
	insertActivateList(object);
	insertDeactivateList(object);
	recalculateObjectsState();
}

void Level::deleteObject(LevelObject* object)
{
	removeObject(object);
	removeActivateList(object);
	removeDeactivateList(object);
	recalculateObjectsState();
}

void Level::insertObject(LevelObject* object)
{
	levelObjects.emplace(object->id, object);
}

void Level::removeObject(LevelObject* object)
{
	levelObjects.erase(object->id);
}

void Level::insertActivateList(LevelObject* object)
{
	activateList.insert(
		std::upper_bound(activateList.begin(), activateList.end(), object,
			[](LevelObject* a, LevelObject* b)
			{
				return a->startTime < b->startTime;
			}), object);
}

void Level::insertDeactivateList(LevelObject* object)
{
	deactivateList.insert(
		std::upper_bound(deactivateList.begin(), deactivateList.end(), object,
			[](LevelObject* a, LevelObject* b)
			{
				return a->endTime < b->endTime;
			}), object);
}

void Level::removeActivateList(LevelObject* object)
{
	activateList.erase(std::remove(activateList.begin(), activateList.end(), object));
}

void Level::removeDeactivateList(LevelObject* object)
{
	deactivateList.erase(std::remove(deactivateList.begin(), deactivateList.end(), object));
}

void Level::recalculateObjectsState()
{
	std::unordered_map<LevelObject*, int> activeTable;
	aliveObjects.clear();

	for (auto pair : levelObjects)
	{
		LevelObject* obj = pair.second;
		activeTable[obj] = 0;
	}

	for (LevelObject* object : activateList)
	{
		if (object->startTime > time)
		{
			break;
		}

		activeTable[object]++;
	}

	for (LevelObject* object : deactivateList)
	{
		if (object->endTime >= time)
		{
			break;
		}

		activeTable[object]--;
	}

	for (auto pair : activeTable)
	{
		LevelObject* obj = pair.first;
		if (pair.second % 2)
		{
			obj->node->setActive(false);
		}
		else
		{
			obj->node->setActive(false);
			aliveObjects.insert(obj);
		}
	}
}

void Level::updateForward()
{
	while (activateIndex < activateList.size() && time >= activateList[activateIndex]->startTime)
	{
		LevelObject* obj = activateList[activateIndex];
		obj->node->setActive(true);
		aliveObjects.insert(obj);
		activateIndex++;
	}

	while (deactivateIndex < deactivateList.size() && time >= deactivateList[deactivateIndex]->endTime)
	{
		LevelObject* obj = deactivateList[deactivateIndex];
		obj->node->setActive(false);
		aliveObjects.erase(obj);
		deactivateIndex++;
	}
}

void Level::updateReverse()
{
	while (deactivateIndex - 1 >= 0 && time < deactivateList[deactivateIndex - 1]->endTime)
	{
		LevelObject* obj = deactivateList[deactivateIndex - 1];
		obj->node->setActive(true);
		aliveObjects.insert(obj);
		deactivateIndex--;
	}

	while (activateIndex - 1 >= 0 && time < activateList[activateIndex - 1]->startTime)
	{
		LevelObject* obj = activateList[activateIndex - 1];
		obj->node->setActive(false);
		aliveObjects.erase(obj);
		activateIndex--;
	}
}
