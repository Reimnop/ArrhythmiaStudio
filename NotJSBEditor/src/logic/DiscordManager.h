#pragma once

#include <discord/discord.h>

class DiscordManager
{
public:
	static DiscordManager* inst;

	DiscordManager();

	void update();
	void updateActivity(discord::Activity activity);
private:
	discord::Core* core;
};