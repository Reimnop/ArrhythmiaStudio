#pragma once

#include <GLFW/glfw3.h>

#include "engine/rendering/Renderer.h"

#ifdef _DEBUG
// #define SHOW_NON_IMPORTANT
#define THROW_GL_ERROR
#define DEBUG_CALLBACK_SYNCHRONOUS
#endif

class GameManager;

class MainWindow
{
public:
	static MainWindow* inst;

	GLFWwindow* window;

	float time;
	float deltaTime;

	MainWindow();
	~MainWindow();

	// Starts main loop
	void run();
private:
	Renderer* renderer;

	GameManager* gameManager;

	void onLoad();
	void onUpdateFrame() const;
	void onRenderFrame() const;
};
