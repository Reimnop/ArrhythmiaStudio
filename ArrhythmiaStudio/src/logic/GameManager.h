#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "log4cxx/logger.h"
#include "DocumentationWindow.h"
#include "Level.h"
#include "editor_windows/EditorWindow.h"
#include "PrefabManagerWindow.h"
#include "VideoExporterWindow.h"

class MainWindow;

class GameManager
{
public:
	static GameManager* inst;

	GameManager(GLFWwindow* window);

	std::unique_ptr<Level> level;

	// Runs every frame
	void update();
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("GameManager");

	GLFWwindow* mainWindow;

    std::unique_ptr<DocumentationWindow> docManager;
    std::unique_ptr<PrefabManagerWindow> prefabManager;
    std::unique_ptr<VideoExporterWindow> videoExporterWindow;

	std::vector<std::unique_ptr<EditorWindow>> editorWindows;

	void onLayout();
};
