#pragma once

#include <string>
#include <unordered_map>

#include "log4cxx/logger.h"
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

		LOG4CXX_INFO(logger, "Loaded level event " << id.c_str());
	}

	static LevelEventInfo getFromId(std::string id)
	{
		return infos[id];
	}
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("LevelEventFactory");
	static inline std::unordered_map<std::string, LevelEventInfo> infos;
};
