#pragma once

#include <discord/discord.h>

class DiscordManager
{
public:
	static DiscordManager* inst;

	DiscordManager();

	void update() const;
	void updateActivity(discord::Activity activity) const;
private:
	discord::Core* core;

	void debugCallback(discord::LogLevel logLevel, const char* msg);
};
