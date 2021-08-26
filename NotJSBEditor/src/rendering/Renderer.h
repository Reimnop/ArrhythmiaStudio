#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "../logic/Scene.h"
#include "OutputDrawData.h"
#include "Camera.h"
#include "ImGuiController.h"
#include "FramebufferStack.h"
#include "PostProcessing/Bloom.h"
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
	void render();

	uint32_t getRenderTexture() const;
private:
	int lastViewportWidth;
	int lastViewportHeight;

	uint32_t lastVertexArray = 0;
	uint32_t lastShader = 0;

	uint32_t framebuffer;
	uint32_t renderTexture;
	uint32_t depthBuffer;

	std::vector<OutputDrawData*> queuedDrawDataOpaque;
	std::vector<OutputDrawData*> queuedDrawDataTransparent;

	GLFWwindow* mainWindow;
	ImGuiController* imGuiController;

	void recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform, glm::mat4 view, glm::mat4 projection);
	void processDrawData(const OutputDrawData* drawData);
};
