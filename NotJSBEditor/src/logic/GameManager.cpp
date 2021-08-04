#include "GameManager.h"
#include "../MainWindow.h"

GameManager::GameManager(GLFWwindow* window)
{
	mainWindow = window;

	levelManager = new LevelManager();
}

void GameManager::update()
{
	levelManager->update(MainWindow::inst->time);
}
