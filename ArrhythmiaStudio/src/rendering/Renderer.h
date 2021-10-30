#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "DrawCommand.h"
#include "../logic/Scene.h"
#include "BatchedDrawData.h"
#include "Camera.h"
#include "ImGuiController.h"
#include "FramebufferStack.h"
#include "post_processing/Bloom.h"
#include "text/SampleTextRenderer.h"
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

	uint32_t lastShader = -1;

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

	bool(*opaqueComp)(const BatchedDrawData*, const BatchedDrawData*) =
		[](const BatchedDrawData* a, const BatchedDrawData* b)
		{
			return a->material < b->material;
		};

	bool(*transparentComp)(const RenderCommand*, const RenderCommand*) = 
		[](const RenderCommand* a, const RenderCommand* b)
		{
			return a->drawDepth < b->drawDepth;
		};

	std::vector<RenderCommand*> queuedCommandOpaque;
	std::vector<RenderCommand*> queuedCommandTransparent;
	std::vector<BatchedDrawData*> queuedBatchedDrawDataOpaque;

	GLFWwindow* mainWindow;
	ImGuiController* imGuiController;

	void recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform);
	void queueCommand(const BatchedDrawData* drawData);
};
