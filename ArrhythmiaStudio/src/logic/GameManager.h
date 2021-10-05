#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "LevelCreateInfo.h"
#include "LevelManager.h"
#include "DocManager.h"
#include "ShapeManager.h"
#include "DataManager.h"
#include "DebugMenu.h"
#include "DiscordManager.h"
#include "UndoRedoManager.h"

class MainWindow;

class GameManager
{
public:
	GameManager(GLFWwindow* window);

	// Runs every frame
	void update() const;
private:
	GLFWwindow* mainWindow;
	LevelManager* levelManager;
	DocManager* docManager;
	ShapeManager* shapeManager;
	DataManager* dataManager;
	DiscordManager* discordManager;
	UndoRedoManager* undoRedoManager;
	DebugMenu* debug;

	bool welcomeOpened = false;
	float editTimeElapsed = 0.0f;

	LevelCreateInfo currentCreateInfo;

	std::string videoPath;
	int videoWidth = 1920;
	int videoHeight = 1080;
	int videoFramerate = 60;
	int videoStartFrame = 0;
	int videoEndFrame = 300;

	void onLayout();
	void calculateViewportRect(ImVec2 size, float* width, float* height);
	std::string timeToString(float time) const;
};
