#pragma once
#include <string>
#include <unordered_map>
#include "../object_behaviours/LevelObjectBehaviour.h"

struct ObjectBehaviourInfo
{
	typedef LevelObjectBehaviour* (*BehaviourCreateFunc)(LevelObject*);

	std::string name;
	BehaviourCreateFunc createFunction;
};

class ObjectBehaviourFactory
{
public:
	ObjectBehaviourFactory() = delete;

	static std::vector<std::string> getBehaviorIds()
	{
		std::vector<std::string> ids;
		ids.reserve(infos.size());
		for (auto kv : infos)
		{
			ids.push_back(kv.first);
		}
		return ids;
	}

	template<typename T>
	static void registerBehaviour(std::string id, std::string name)
	{
		ObjectBehaviourInfo info;
		info.name = name;
		info.createFunction = &T::create;
		infos.emplace(id, info);
	}

	static ObjectBehaviourInfo getFromId(std::string id)
	{
		return infos[id];
	}
private:
	static inline std::unordered_map<std::string, ObjectBehaviourInfo> infos;
};
