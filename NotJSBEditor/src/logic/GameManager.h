#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "LevelManager.h"

class MainWindow;

class GameManager
{
public:
	GameManager(GLFWwindow* window);

	// Runs every frame
	void update();
private:
	GLFWwindow* mainWindow;
	LevelManager* levelManager;

	void onLayout();
	void calculateViewportRect(ImVec2 size, float* width, float* height);
};
