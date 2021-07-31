#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <helper.h>
#include <logger.h>

#include "rendering/Renderer.h"
#include "logic/GameManager.h"

#ifndef NDEBUG
// #define SHOW_NON_IMPORTANT
#define THROW_GL_ERROR
#endif

class MainWindow {
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
	void onUpdateFrame();
	void onRenderFrame();
};