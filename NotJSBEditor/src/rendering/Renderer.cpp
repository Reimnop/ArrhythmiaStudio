#include "Renderer.h"

#include "DrawElementsCommand.h"
#include "PostProcessing/Bloom.h"
#include "GlobalConstants.h"

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

	viewportWidth = RENDERER_INITIAL_WIDTH;
	viewportHeight = RENDERER_INITIAL_HEIGHT;
	lastViewportWidth = viewportWidth;
	lastViewportHeight = viewportHeight;

	// Generate render texture
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Initialize framebuffer to copy from multisample framebuffer
	glGenFramebuffers(1, &finalFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, finalFramebuffer);

	// Attach generated resources to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Generate multisample color texture
	glGenTextures(1, &multisampleTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_R11F_G11F_B10F, viewportWidth, viewportHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// Generate render texture
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH_COMPONENT, viewportWidth, viewportHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Initialize framebuffer to render into
	glGenFramebuffers(1, &multisampleFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer);

	// Attach generated resources to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mainWindow = window;

	camera = new Camera();
	imGuiController = new ImGuiController(window, "Assets/Inconsolata.ttf");

	bloom = new Bloom();
	// tonemapping = new Tonemapping();
}

void Renderer::update()
{
	imGuiController->update();

	if (viewportWidth != lastViewportWidth || viewportHeight != lastViewportHeight)
	{
		// Resize all framebuffer attachments
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, nullptr);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTexture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_R11F_G11F_B10F, viewportWidth, viewportHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH_COMPONENT, viewportWidth, viewportHeight);
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

	FramebufferStack::push(multisampleFramebuffer);

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

	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFramebuffer);

	glBlitFramebuffer(0, 0, viewportWidth, viewportHeight, 0, 0, viewportWidth, viewportHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bloom->processImage(renderTexture, viewportWidth, viewportHeight);
	// tonemapping->processImage(renderTexture, viewportWidth, viewportHeight);

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
				std::vector<OutputDrawData*>::iterator it = std::lower_bound(
					queuedDrawDataTransparent.begin(), queuedDrawDataTransparent.end(), output,
					[](const OutputDrawData* a, const OutputDrawData* b)
					{
						return a->drawDepth < b->drawDepth;
					});

				queuedDrawDataTransparent.insert(it, output);
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
