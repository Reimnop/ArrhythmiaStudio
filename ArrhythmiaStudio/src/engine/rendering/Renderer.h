#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "../Scene.h"
#include "BatchedDrawData.h"
#include "../Camera.h"
#include "ImGuiController.h"
#include "FramebufferStack.h"
#include "post_processing/Bloom.h"
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
