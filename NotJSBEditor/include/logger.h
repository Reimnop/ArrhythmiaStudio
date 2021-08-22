#pragma once

#include <iostream>

static class Logger
{
public:
	static void info(std::string msg) {
		std::cout << "\u001b[40m" /* BG */ << "\u001b[97m" /* FG */ << "[NotJSBEditor] [INFO] " << msg << std::endl;
	}

	static void warn(std::string msg) {
		std::cout << "\u001b[43m" /* BG */ << "\u001b[37m" /* FG */ << "[NotJSBEditor] [WARN] " << "\u001b[0m" /* RESET */ << " " << "\u001b[93m" /* FG */ << msg << std::endl;
	}

	static void error(std::string msg) {
		std::cout << "\u001b[41m" /* BG */ << "\u001b[37m" /* FG */ << "[NotJSBEditor] [ERROR] " << "\u001b[0m" /* RESET */ << " " << "\u001b[91m" /* FG */ << msg << std::endl;
	}
};