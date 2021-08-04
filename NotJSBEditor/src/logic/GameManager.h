#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Scene.h"
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
};
