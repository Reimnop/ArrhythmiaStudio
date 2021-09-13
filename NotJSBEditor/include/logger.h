#pragma once

#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <fstream>

class Logger
{
public:
	Logger() = delete;

	static void initLog()
	{
		PWSTR path_tmp;
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path_tmp);

		std::filesystem::path path = path_tmp;

		path = path / "Reimnop/NotJSBEditor/Logs";
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
		std::cout << "[NotJSBEditor] [INFO]";

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout << " " << obj << std::endl;

		logStream << "[NotJSBEditor] [INFO] " << obj << std::endl;
	}

	template<typename T>
	static void warn(T obj) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout << "[NotJSBEditor] [WARN]";

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << " " << obj << std::endl;

		logStream << "[NotJSBEditor] [WARN] " << obj << std::endl;
	}

	template<typename T>
	static void error(T obj) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout << "[NotJSBEditor] [ERRO]";

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << " " << obj << std::endl;

		logStream << "[NotJSBEditor] [ERRO] " << obj << std::endl;
	}
private:
	static inline std::ofstream logStream;
};