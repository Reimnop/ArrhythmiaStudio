#pragma once

#include <discord/discord.h>

#include "log4cxx/logger.h"

class DiscordManager
{
public:
	static DiscordManager* inst;

	DiscordManager();

	void update() const;
	void updateActivity(discord::Activity activity) const;
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("DiscordManager");

	discord::Core* core;

	void debugCallback(discord::LogLevel logLevel, const char* msg);
};
