#pragma once

#include <string>
#include <unordered_map>

#include "../level_events/LevelEvent.h"

struct LevelEventInfo
{
	typedef LevelEvent* (*EventCreateFunc)(Level*);

	std::string name;
	EventCreateFunc createFunction;
};

class LevelEventFactory
{
public:
	LevelEventFactory() = delete;

	static std::vector<std::string> getEventIds()
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
	static void registerEvent(std::string id, std::string name)
	{
		LevelEventInfo info;
		info.name = name;
		info.createFunction = &T::create;
		infos.emplace(id, info);
	}

	static LevelEventInfo getFromId(std::string id)
	{
		return infos[id];
	}
private:
	static inline std::unordered_map<std::string, LevelEventInfo> infos;
};
