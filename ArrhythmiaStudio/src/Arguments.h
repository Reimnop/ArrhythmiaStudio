#pragma once

#include <unordered_map>
#include <string>

#include "log4cxx/logger.h"

class Arguments
{
public:
	Arguments() = delete;

	static void parseArguments(int argc, char** argv)
	{
		for (int i = 0; i < argc; i++)
		{
			std::string str(argv[i]);

			if (*argv[i] == '-')
			{
				std::string argName(argv[i] + 1);
				std::string argValue(argv[i + 1]);
				arguments.emplace(argName, argValue);
			}
		}
	}

	static bool tryGet(std::string name, std::string* value)
	{
		if (arguments.contains(name))
		{
			*value = arguments[name];
			return true;
		}
		return false;
	}

	static std::string get(std::string name)
	{
		std::string value;
		LOG4CXX_ASSERT(log4cxx::Logger::getRootLogger(), tryGet(name, &value), "Argument \"" << name << "\" not specified!");
		return value;
	}
private:
	static inline std::unordered_map<std::string, std::string> arguments;
};
