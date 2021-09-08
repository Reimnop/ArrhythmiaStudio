#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "Batch.h"
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

	// Batch rendering buffers
	uint32_t batchVAO;
	uint32_t batchVBO;
	uint32_t batchEBO;

	size_t lastVertexBatchBufferSize;
	size_t lastIndexBatchBufferSize;

	std::vector<Batch> batches;
	std::vector<glm::vec3> batchVertexBuffer;
	std::vector<uint32_t> batchIndexBuffer;

	std::vector<GLsizei> batchCounts;
	std::vector<void*> batchIndices;
	std::vector<GLint> batchBaseVertices;

	std::vector<OutputDrawData*> queuedDrawDataOpaque;
	std::vector<OutputDrawData*> queuedDrawDataTransparent;

	GLFWwindow* mainWindow;
	ImGuiController* imGuiController;

	void recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform);
	void addToBatch(const OutputDrawData* drawData);
};
