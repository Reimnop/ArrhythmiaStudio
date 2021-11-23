#pragma once
#include <string>
#include <unordered_map>
#include "../object_behaviours/LevelObjectBehaviour.h"

class ObjectBehaviourFactory
{
public:
	template<typename T>
	static void registerBehaviour(std::string id)
	{
		createFuncs.emplace(id, &T::create);
	}

	static LevelObjectBehaviour* getFromId(std::string id, LevelObject* object)
	{
		return createFuncs[id](object);
	}
private:
	typedef LevelObjectBehaviour* (*BehaviourCreateFunc)(LevelObject*);

	static inline std::unordered_map<std::string, BehaviourCreateFunc> createFuncs;
};
