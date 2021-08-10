#pragma once

#include <GLFW/glfw3.h>

#include "rendering/Renderer.h"

#ifndef NDEBUG
// #define SHOW_NON_IMPORTANT
#define THROW_GL_ERROR
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
