#include "MainWindow.h"

void glfwErrorCallback(int error_code, const char* description)
{
	Logger::error("GLFW: " + std::string(description));
}

void APIENTRY glDebugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                              const char* message, const void* userParam)
{
	std::string msgStr = std::string(message);

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
		throw msgStr;
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
	if (MainWindow::inst)
	{
		return;
	}

	MainWindow::inst = this;

	glfwSetErrorCallback(glfwErrorCallback);

	// Init GLFW
	if (!glfwInit())
	{
		EXIT_FATAL("GLFW initialization failed!");
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
		EXIT_FATAL("Window creation failed!");
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		EXIT_FATAL("GLAD initialization failed!");
	}

	// Intialize OpenGL debug callback
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugCallback, nullptr);

	// Initialize other things with onLoad
	onLoad();
}

MainWindow::~MainWindow()
{
	glfwTerminate();
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

void MainWindow::onUpdateFrame()
{
	gameManager->update();

	// Update renderer last
	renderer->update();
}

void MainWindow::onRenderFrame()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer->render();

	glfwSwapBuffers(window);
}
