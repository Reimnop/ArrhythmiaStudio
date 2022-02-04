#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "log4cxx/logger.h"
#include "DocumentationWindow.h"
#include "Level.h"
#include "editor_windows/EditorWindow.h"

class MainWindow;

class GameManager
{
public:
	static GameManager* inst;

	GameManager(GLFWwindow* window);

	Level* level;

	// Runs every frame
	void update();
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("GameManager");

	GLFWwindow* mainWindow;
	DocumentationWindow* docManager;

	std::vector<EditorWindow*> editorWindows;

	void onLayout();
};
