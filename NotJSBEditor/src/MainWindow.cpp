#include "MainWindow.h"
#include "logic/GameManager.h"

#include <logger.h>
#include <bass/bass.h>

void glfwErrorCallback(int error_code, const char* description)
{
	Logger::error("GLFW: " + std::string(description));
}

void glDebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
	std::string msgStr = std::string(message, length);

	switch (severity)
	{
#ifdef SHOW_NON_IMPORTANT
	case GL_DEBUG_SEVERITY_LOW:
		Logger::info("OpenGL: " + msgStr);
		break;
#endif
	case GL_DEBUG_SEVERITY_MEDIUM:
		Logger::warn("OpenGL: " + msgStr);
		break;
	case GL_DEBUG_SEVERITY_HIGH:
#ifdef THROW_GL_ERROR
		throw std::runtime_error(msgStr);
#else
		Logger::error("OpenGL: " + msgStr);
#endif
		break;
#ifdef SHOW_NON_IMPORTANT
	default:
		Logger::info("OpenGL: " + msgStr);
		break;
#endif
	}
}

MainWindow* MainWindow::inst;

MainWindow::MainWindow()
{
	if (inst)
	{
		return;
	}

	inst = this;

	glfwSetErrorCallback(glfwErrorCallback);

	// Init GLFW
	if (!glfwInit())
	{
		throw std::runtime_error("GLFW initialization failed!");
	}

	// Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(1600, 900, "Not JSB Editor", NULL, NULL);

	if (!window)
	{
		throw std::runtime_error("Window creation failed!");
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("GLAD initialization failed!");
	}

	glfwSwapInterval(1);

	// Intialize OpenGL debug callback
	glDebugMessageCallback(glDebugCallback, nullptr);

	glEnable(GL_DEBUG_OUTPUT);
#ifdef DEBUG_CALLBACK_SYNCHRONOUS
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	// Initialize audio
	BASS_Init(-1, 44100, BASS_DEVICE_STEREO, NULL, NULL);

	// Initialize other things with onLoad
	onLoad();
}

MainWindow::~MainWindow()
{
	glfwTerminate();
	BASS_Free();
}

void MainWindow::onLoad()
{
	new Scene();

	renderer = new Renderer(window);
	gameManager = new GameManager(window);
}

void MainWindow::run()
{
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = glfwGetTime();

		deltaTime = currentTime - time;
		time = currentTime;

		glfwPollEvents();

		onUpdateFrame();
		onRenderFrame();
	}
}

void MainWindow::onUpdateFrame() const
{
	gameManager->update();

	// Update renderer last
	renderer->update();
}

void MainWindow::onRenderFrame() const
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer->render();

	glfwSwapBuffers(window);
}
