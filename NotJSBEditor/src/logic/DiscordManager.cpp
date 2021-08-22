#include "DiscordManager.h"

#include <logger.h>

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
		Logger::error("Error while initializing Discord!");
	}
	else 
	{
		core->SetLogHook(discord::LogLevel::Debug, [this](discord::LogLevel logLevel, const char* msg) { this->debugCallback(logLevel, msg); });
	}
}

void DiscordManager::updateActivity(discord::Activity activity)
{
	if (core) 
	{
		core->ActivityManager().UpdateActivity(activity, nullptr);
	}
}

void DiscordManager::update()
{
	if (core)
	{
		core->RunCallbacks();
	}
}

void DiscordManager::debugCallback(discord::LogLevel logLevel, const char* msg)
{
	std::string msgStr(msg);

	switch (logLevel)
	{
	case discord::LogLevel::Debug:
		Logger::info("Discord: " + msgStr);
	case discord::LogLevel::Info:
		Logger::info("Discord: " + msgStr);
	case discord::LogLevel::Warn:
		Logger::warn("Discord: " + msgStr);
	case discord::LogLevel::Error:
		Logger::error("Discord: " + msgStr);
	}
}
