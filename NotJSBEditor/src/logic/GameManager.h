#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "LevelCreateInfo.h"
#include "LevelManager.h"
#include "ShapeManager.h"
#include "DataManager.h"
#include "DiscordManager.h"

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
	ShapeManager* shapeManager;
	DataManager* dataManager;
	DiscordManager* discordManager;

	bool welcomeOpened = false;
	float editTimeElapsed = 0.0f;

	LevelCreateInfo currentCreateInfo;

	void onLayout();
	void calculateViewportRect(ImVec2 size, float* width, float* height);
	std::string timeToString(float time);
};
