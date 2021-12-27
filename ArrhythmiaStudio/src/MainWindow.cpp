#include "MainWindow.h"
#include "logic/GameManager.h"

#include "helper.h"
#include "bass/bass.h"

void MainWindow::glfwErrorCallback(int error_code, const char* description)
{
	LOG4CXX_ERROR(logger, "GLFW: " << description);
}

void MainWindow::glDebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
	std::string msgStr = std::string(message, length);

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:
		LOG4CXX_DEBUG(logger, "OpenGL: " << msgStr.c_str());
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		LOG4CXX_WARN(logger, "OpenGL: " << msgStr.c_str());
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		LOG4CXX_ERROR(logger, "OpenGL: " << msgStr.c_str());
		break;
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
	LOG4CXX_ASSERT(logger, glfwInit(), "GLFW initialization failed!");

	// Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

	window = glfwCreateWindow(1600, 900, STRINGIFY(PROJECT_NAME), NULL, NULL);

	LOG4CXX_ASSERT(logger, window, "Window creation failed!");

	glfwMakeContextCurrent(window);

	LOG4CXX_ASSERT(logger, gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "GLAD initialization failed!");

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
