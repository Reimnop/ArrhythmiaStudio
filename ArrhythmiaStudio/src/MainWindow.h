#pragma once

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "engine/rendering/Renderer.h"

#ifdef _DEBUG
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
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("MainWindow");

	Renderer* renderer;

	GameManager* gameManager;

	void onLoad();
	void onUpdateFrame() const;
	void onRenderFrame() const;

	static void glfwErrorCallback(int error_code, const char* description);
	static void glDebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
};
