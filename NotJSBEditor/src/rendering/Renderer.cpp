#include "Renderer.h"

#include "DrawElementsCommand.h"

#include <algorithm>
#include <logger.h>

Renderer* Renderer::inst;

Renderer::Renderer(GLFWwindow* window)
{
	if (inst)
	{
		return;
	}
	inst = this;

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* langVer = glGetString(GL_SHADING_LANGUAGE_VERSION);

	Logger::info("Renderer: " + std::string((const char*)renderer));
	Logger::info("Vendor: " + std::string((const char*)vendor));
	Logger::info("OpenGL Version: " + std::string((const char*)version));
	Logger::info("GLSL Version: " + std::string((const char*)langVer));

	constexpr int initialWidth = 1280;
	constexpr int initialHeight = 720;

	viewportWidth = initialWidth;
	viewportHeight = initialHeight;
	lastViewportWidth = initialWidth;
	lastViewportHeight = initialHeight;

	// Generate color texture
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, initialWidth, initialHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate render texture
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, initialWidth, initialHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Initialize framebuffer to render into
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Attach generated resources to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mainWindow = window;

	camera = new Camera();
	imGuiController = new ImGuiController(window, "Assets/Inconsolata.ttf");
}

void Renderer::update()
{
	imGuiController->update();

	if (viewportWidth != lastViewportWidth || viewportHeight != lastViewportHeight)
	{
		// Resize all framebuffer attachments
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewportWidth, viewportHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		lastViewportWidth = viewportWidth;
		lastViewportHeight = viewportHeight;
	}
}

void Renderer::render()
{
	float aspect = viewportWidth / (float)viewportHeight;
	glm::mat4 view, projection;
	camera->calculateViewProjection(aspect, &view, &projection);
	recursivelyRenderNodes(Scene::inst->rootNode, glm::mat4(1.0f), view, projection);

	FramebufferStack::push(framebuffer);

	glViewport(0, 0, viewportWidth, viewportHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	for (const OutputDrawData* drawData : queuedDrawDataOpaque)
	{
		processDrawData(drawData);

		delete drawData;
	}

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (const OutputDrawData* drawData : queuedDrawDataTransparent)
	{
		processDrawData(drawData);

		delete drawData;
	}

	glDepthMask(GL_TRUE);

	FramebufferStack::pop();

	// Clean up
	queuedDrawDataOpaque.clear();
	queuedDrawDataTransparent.clear();

	lastVertexArray = 0;
	lastShader = 0;

	glBindVertexArray(0);
	glUseProgram(0);

	// Render ImGui last
	imGuiController->renderImGui();
}

uint32_t Renderer::getRenderTexture() const
{
	return renderTexture;
}

void Renderer::recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform, glm::mat4 view, glm::mat4 projection)
{
	if (!node->getActive())
	{
		return;
	}

	const glm::mat4 nodeTransform = node->transform->getLocalMatrix();
	const glm::mat4 globalTransform = parentTransform * nodeTransform;

	if (node->renderer)
	{
		InputDrawData drawData;
		drawData.model = globalTransform;
		drawData.view = view;
		drawData.projection = projection;
		drawData.modelViewProjection = projection * view * globalTransform;

		OutputDrawData* output;
		if (node->renderer->render(drawData, &output))
		{
			if (output->drawTransparent)
			{
				if (queuedDrawDataTransparent.empty())
				{
					queuedDrawDataTransparent.push_back(output);
				}
				else
				{
					std::vector<OutputDrawData*>::iterator it = std::lower_bound(
						queuedDrawDataTransparent.begin(), queuedDrawDataTransparent.end(), output,
						[](const OutputDrawData* a, const OutputDrawData* b)
						{
							return a->drawDepth < b->drawDepth;
						});

					queuedDrawDataTransparent.insert(it, output);
				}
			}
			else
			{
				queuedDrawDataOpaque.push_back(output);
			}
		}
	}

	for (SceneNode* child : node->activeChildren)
	{
		recursivelyRenderNodes(child, globalTransform, view, projection);
	}
}

void Renderer::processDrawData(const OutputDrawData* drawData)
{
	if (drawData->vao != lastVertexArray)
	{
		glBindVertexArray(drawData->vao);
		lastVertexArray = drawData->vao;
	}

	if (drawData->shader != lastShader)
	{
		glUseProgram(drawData->shader);
		lastShader = drawData->shader;
	}

	// Bind all uniform buffers
	if (drawData->uniformBuffers)
	{
		for (int i = 0; i < drawData->uniformBuffersCount; i++)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, i, drawData->uniformBuffers[i]);
		}
	}

	drawData->renderCallback();

	switch (drawData->commandType)
	{
	case DrawCommandType_DrawElements:
		const DrawElementsCommand* drawElementsCommand = (DrawElementsCommand*)drawData->drawCommand;
		glDrawElements(GL_TRIANGLES, drawElementsCommand->count, GL_UNSIGNED_INT, (void*)drawElementsCommand->offset);
		break;
	}

	delete drawData->drawCommand;
}
