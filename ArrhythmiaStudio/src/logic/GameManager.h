#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "DocManager.h"
#include "DiscordManager.h"
#include "editor_windows/EditorWindow.h"

class MainWindow;

class GameManager
{
public:
	GameManager(GLFWwindow* window);

	// Runs every frame
	void update();
private:
	GLFWwindow* mainWindow;
	DocManager* docManager;
	DiscordManager* discordManager;

	std::vector<EditorWindow*> editorWindows;

	bool welcomeOpened = false;
	float editTimeElapsed = 0.0f;

	void onLayout();
};
