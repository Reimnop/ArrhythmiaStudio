#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <functional>

#include "Scene.h"
#include "LevelManager.h"
#include "../MainWindow.h"
#include "../rendering/ImGuiController.h"
#include "../rendering/Renderer.h"

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
