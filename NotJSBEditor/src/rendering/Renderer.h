#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "../logic/Scene.h"
#include "OutputDrawData.h"
#include "Camera.h"
#include "ImGuiController.h"
#include "DrawElementsCommand.h"

class Renderer {
public:
	Renderer(GLFWwindow* window);
	void update();
	void render();
private:
	uint32_t lastVertexArray = 0;
	uint32_t lastShader = 0;

	std::vector<OutputDrawData> queuedDrawData;

	GLFWwindow* mainWindow;
	ImGuiController* imGuiController;

	Camera* camera;

	void recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform, glm::mat4 view, glm::mat4 projection);
};