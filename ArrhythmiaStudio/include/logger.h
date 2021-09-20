#pragma once

#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <fstream>

#define LOGGER_STR(x) #x
#define LOGGER_XSTR(x) LOGGER_STR(x)

#define LOGGER_PATH Reimnop/PROJECT_NAME/Logs
#define LOGGER_INFO [PROJECT_NAME] [INFO]
#define LOGGER_WARN [PROJECT_NAME] [WARN]
#define LOGGER_ERRO [PROJECT_NAME] [ERRO]

class Logger
{
public:
	Logger() = delete;

	static void initLog()
	{
		PWSTR path_tmp;
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path_tmp);

		std::filesystem::path path = path_tmp;

		path = path / LOGGER_XSTR(LOGGER_PATH);
		create_directories(path);

		const time_t time = std::time(nullptr);

		struct tm buf;
		localtime_s(&buf, &time);

		std::stringstream ss;
		ss << std::put_time(&buf, "%Y-%m-%d %H-%M-%S") << ".txt";

		logStream = std::ofstream(path / ss.str());
	}

	template<typename T>
	static void info(T obj) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
		std::cout << LOGGER_XSTR(LOGGER_INFO);

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout << " " << obj << std::endl;

		logStream << LOGGER_XSTR(LOGGER_INFO) << " " << obj << std::endl;
	}

	template<typename T>
	static void warn(T obj) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout << LOGGER_XSTR(LOGGER_WARN);

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << " " << obj << std::endl;

		logStream << LOGGER_XSTR(LOGGER_WARN) << " " << obj << std::endl;
	}

	template<typename T>
	static void error(T obj) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout << LOGGER_XSTR(LOGGER_ERRO);

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << " " << obj << std::endl;

		logStream << LOGGER_XSTR(LOGGER_ERRO) << " " << obj << std::endl;
	}
private:
	static inline std::ofstream logStream;
};