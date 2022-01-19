#pragma once

#include <vector>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "../Scene.h"
#include "../Camera.h"
#include "ImGuiController.h"
#include "FramebufferStack.h"
#include "post_processing/Bloom.h"
#include "log4cxx/logger.h"
// #include "PostProcessing/Tonemapping.h"

class Renderer
{
public:
	static Renderer* inst;

	Bloom* bloom;
	// Tonemapping* tonemapping;

	Camera* camera;

	int viewportWidth;
	int viewportHeight;

	Renderer(GLFWwindow* window);
	void update();
	void resizeViewport();
	void render();
	void renderViewport();

	uint32_t getRenderTexture() const;
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Renderer");

	int lastViewportWidth;
	int lastViewportHeight;

	uint32_t finalFramebuffer;
	uint32_t renderTexture;

	uint32_t multisampleFramebuffer;
	uint32_t multisampleTexture;
	uint32_t depthBuffer;

	std::vector<RenderCommand*> queuedCommandOpaque;
	std::vector<RenderCommand*> queuedCommandTransparent;

	std::vector<Drawer*> queuedOpaqueDrawers;
	std::vector<Drawer*> queuedTransparentDrawers;

	Drawer* currentDrawer;

	GLFWwindow* mainWindow;
	ImGuiController* imGuiController;

	void recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform);
	void prepareDrawers();
};
