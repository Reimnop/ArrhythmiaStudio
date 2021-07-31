#include "GameManager.h"
#include "../MainWindow.h"

GameManager::GameManager(GLFWwindow* window, Scene* scene) {
	mainWindow = window;
	mainScene = scene;

	LevelManager* levelManager = new LevelManager();
}

void GameManager::update() {
	
}