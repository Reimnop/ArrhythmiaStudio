#pragma once

#include <unordered_map>

#include "log4cxx/logger.h"
#include "../../engine/rendering/text/Font.h"

using namespace std::filesystem;

class FontFactory
{
public:
	FontFactory() = delete;

	static std::vector<std::string> getFontIds()
	{
		std::vector<std::string> ids;
		ids.reserve(fonts.size());
		for (auto kv : fonts)
		{
			ids.push_back(kv.first);
		}
		return ids;
	}

	static void registerFont(path path, std::string id, std::string name)
	{
		Font* font = new Font(path, name);
		fonts[id] = font;

		LOG4CXX_INFO(logger, "Loaded font " << id.c_str());
	}

	static Font* getFont(std::string id)
	{
		return fonts[id];
	}
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("FontFactory");
	static inline std::unordered_map<std::string, Font*> fonts;
};
