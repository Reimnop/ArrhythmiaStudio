#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <queue>

#include "DrawCommand.h"
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

	uint32_t finalFramebuffer;
	uint32_t renderTexture;

	uint32_t multisampleFramebuffer;
	uint32_t multisampleTexture;
	uint32_t depthBuffer;

	// MultiDraw rendering
	uint32_t mdVAO;
	uint32_t mdVBO;
	uint32_t mdEBO;

	size_t lastVertexBufferSize;
	size_t lastIndexBufferSize;

	std::vector<DrawCommand> commands;
	std::vector<glm::vec3> cmdVertexBuffer;
	std::vector<uint32_t> cmdIndexBuffer;

	std::vector<GLsizei> cmdCounts;
	std::vector<void*> cmdIndices;
	std::vector<GLint> cmdBaseVertices;

	bool(*opaqueComp)(const OutputDrawData*, const OutputDrawData*) =
		[](const OutputDrawData* a, const OutputDrawData* b)
		{
			return a->material < b->material;
		};

	bool(*transparentComp)(const OutputDrawData*, const OutputDrawData*) = 
		[](const OutputDrawData* a, const OutputDrawData* b)
		{
			return a->drawDepth < b->drawDepth;
		};

	std::vector<OutputDrawData*> queuedDrawDataOpaque;
	std::vector<OutputDrawData*> queuedDrawDataTransparent;

	GLFWwindow* mainWindow;
	ImGuiController* imGuiController;

	void recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform);
	void queueCommand(const OutputDrawData* drawData);
};
