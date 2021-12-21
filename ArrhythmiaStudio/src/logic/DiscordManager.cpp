#include "DiscordManager.h"

DiscordManager* DiscordManager::inst;

DiscordManager::DiscordManager()
{
	if (inst)
	{
		return;
	}
	inst = this;

	discord::Result result = discord::Core::Create(878313341439864882, DiscordCreateFlags_NoRequireDiscord, &core);

	if (result != discord::Result::Ok)
	{
		LOG4CXX_ERROR(logger, "Error while initializing Discord!");
	}
	else
	{
		core->SetLogHook(discord::LogLevel::Debug, [this](discord::LogLevel logLevel, const char* msg)
		{
			this->debugCallback(logLevel, msg);
		});
	}
}

void DiscordManager::updateActivity(discord::Activity activity) const
{
	if (core)
	{
		core->ActivityManager().UpdateActivity(activity, nullptr);
	}
	else
	{
		LOG4CXX_ERROR(logger, "Error while updating Discord activity!");
	}
}

void DiscordManager::update() const
{
	if (core)
	{
		core->RunCallbacks();
	}
}

void DiscordManager::debugCallback(discord::LogLevel logLevel, const char* msg)
{
	switch (logLevel)
	{
	case discord::LogLevel::Debug:
		LOG4CXX_DEBUG(logger, "Discord: " << msg);
	case discord::LogLevel::Info:
		LOG4CXX_INFO(logger, "Discord: " << msg);
	case discord::LogLevel::Warn:
		LOG4CXX_WARN(logger, "Discord: " << msg);
	case discord::LogLevel::Error:
		LOG4CXX_ERROR(logger, "Discord: " << msg);
	}
}
