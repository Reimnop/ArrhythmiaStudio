#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "DocManager.h"
#include "DiscordManager.h"

class MainWindow;

class GameManager
{
public:
	GameManager(GLFWwindow* window);

	// Runs every frame
	void update() const;
private:
	GLFWwindow* mainWindow;
	DocManager* docManager;
	DiscordManager* discordManager;

	bool welcomeOpened = false;
	float editTimeElapsed = 0.0f;

	void onLayout();
	void calculateViewportRect(ImVec2 size, float* width, float* height);
	std::string timeToString(float time) const;
};
