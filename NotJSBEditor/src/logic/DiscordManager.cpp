#include "DiscordManager.h"

DiscordManager* DiscordManager::inst;

DiscordManager::DiscordManager()
{
	if (inst)
	{
		return;
	}

	inst = this;

	discord::Core::Create(878313341439864882, DiscordCreateFlags_Default, &core);
}

void DiscordManager::updateActivity(discord::Activity activity)
{
	core->ActivityManager().UpdateActivity(activity, nullptr);
}

void DiscordManager::update()
{
	core->RunCallbacks();
}
