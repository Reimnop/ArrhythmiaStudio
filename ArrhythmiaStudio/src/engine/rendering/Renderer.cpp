#include "Renderer.h"

#include "post_processing/Bloom.h"
#include "GlobalConstants.h"

#include <logger.h>

#include "../../MainWindow.h"
#include "drawers/BatchedDrawer.h"

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
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
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA16F, viewportWidth, viewportHeight, GL_TRUE);
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
	imGuiController = new ImGuiController(window, "Assets/Inconsolata-Regular.ttf");

	bloom = new Bloom();
	// tonemapping = new Tonemapping();
}

void Renderer::update()
{
	imGuiController->update();

	resizeViewport();
}

void Renderer::resizeViewport()
{
	if (viewportWidth != lastViewportWidth || viewportHeight != lastViewportHeight)
	{
		// Resize all framebuffer attachments
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, nullptr);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTexture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA16F, viewportWidth, viewportHeight, GL_TRUE);
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
	renderViewport();

	// Render ImGui last
	imGuiController->renderImGui();
}

void Renderer::renderViewport()
{
	float aspect = viewportWidth / (float)viewportHeight;
	glm::mat4 view, projection;
	camera->calculateViewProjection(aspect, &view, &projection);
	glm::mat4 viewProjection = projection * view;

	recursivelyRenderNodes(Scene::inst->rootNode, glm::mat4(1.0f));
	prepareDrawers();

	FramebufferStack::push(multisampleFramebuffer);

	glViewport(0, 0, viewportWidth, viewportHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// Opaque shits are drawn here
	for (Drawer* drawer : queuedOpaqueDrawers)
	{
		drawer->draw(viewProjection);

		delete drawer;
	}

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Transparent shits are drawn here
	for (Drawer* drawer : queuedTransparentDrawers)
	{
		drawer->draw(viewProjection);

		delete drawer;
	}

	glDepthMask(GL_TRUE);

	FramebufferStack::pop();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFramebuffer);

	glBlitFramebuffer(0, 0, viewportWidth, viewportHeight, 0, 0, viewportWidth, viewportHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bloom->processImage(renderTexture, viewportWidth, viewportHeight);
	// tonemapping->processImage(renderTexture, viewportWidth, viewportHeight);

	// Clean up
	queuedOpaqueDrawers.clear();
	queuedTransparentDrawers.clear();

	glBindVertexArray(0);
	glUseProgram(0);
}

uint32_t Renderer::getRenderTexture() const
{
	return renderTexture;
}

void Renderer::recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform)
{
	if (!node->getActive())
	{
		return;
	}

	const glm::mat4 nodeTransform = node->transform->getLocalMatrix();
	const glm::mat4 globalTransform = parentTransform * nodeTransform;

	if (node->renderer)
	{
		RenderCommand* cmd;
		if (node->renderer->tryRender(globalTransform, &cmd))
		{
			if (cmd->drawDepth.has_value())
			{
				queuedCommandTransparent.push_back(cmd);
			}
			else
			{
				queuedCommandOpaque.push_back(cmd);
			}
		}
	}

	for (SceneNode* child : node->activeChildren)
	{
		recursivelyRenderNodes(child, globalTransform);
	}
}

void Renderer::prepareDrawers()
{
	// Sort draw commands
	// We sort opaque commands by type to maximize batching efficiency
	std::sort(queuedCommandOpaque.begin(), queuedCommandOpaque.end(), [](RenderCommand* a, RenderCommand* b)
		{
			return a->drawData->getType() < b->drawData->getType();
		});

	// For transparent commands, we sort by depth, then by type
	std::sort(queuedCommandTransparent.begin(), queuedCommandTransparent.end(), [](RenderCommand* a, RenderCommand* b)
		{
			assert(a->drawDepth.has_value());
			assert(b->drawDepth.has_value());

			const float depthA = a->drawDepth.value();
			const float depthB = b->drawDepth.value();
			if (depthA != depthB)
			{
				return depthA < depthB;
			}

			return a->drawData->getType() < b->drawData->getType();
		});

	// Batching and queuing other stuff

	// Batching opaque objects
	{
		BatchedDrawer* currentOpaqueDrawer = nullptr;
		for (RenderCommand* cmd : queuedCommandOpaque)
		{
			if (cmd->drawData->getType() == DrawDataType_Batched)
			{
				if (!currentOpaqueDrawer)
				{
					currentOpaqueDrawer = new BatchedDrawer();
				}

				BatchedDrawData* drawData = (BatchedDrawData*)cmd->drawData;
				currentOpaqueDrawer->appendMesh(drawData->mesh, drawData->transform, glm::vec4(1.0f));
			}
			else
			{
				if (currentOpaqueDrawer)
				{
					queuedOpaqueDrawers.push_back(currentOpaqueDrawer);
					currentOpaqueDrawer = nullptr;
				}

				queuedOpaqueDrawers.push_back(cmd->drawData->getDrawer());
			}

			delete cmd;
		}

		// We check if there is any unqueued opaque drawer at the end and if there is, queue it
		if (currentOpaqueDrawer)
		{
			queuedOpaqueDrawers.push_back(currentOpaqueDrawer);
			currentOpaqueDrawer = nullptr;
		}
	}

	// Batching transparent objects
	{
		BatchedDrawer* currentTransparentDrawer = nullptr;
		for (RenderCommand* cmd : queuedCommandTransparent)
		{
			if (cmd->drawData->getType() == DrawDataType_Batched)
			{
				if (!currentTransparentDrawer)
				{
					currentTransparentDrawer = new BatchedDrawer();
				}

				BatchedDrawData* drawData = (BatchedDrawData*)cmd->drawData;
				currentTransparentDrawer->appendMesh(drawData->mesh, drawData->transform, glm::vec4(1.0f, 1.0f, 1.0f, drawData->opacity));
			}
			else
			{
				if (currentTransparentDrawer)
				{
					queuedTransparentDrawers.push_back(currentTransparentDrawer);
					currentTransparentDrawer = nullptr;
				}

				queuedTransparentDrawers.push_back(cmd->drawData->getDrawer());
			}

			delete cmd;
		}

		// We check if there is any unqueued transparent drawer at the end and if there is, queue it
		if (currentTransparentDrawer)
		{
			queuedTransparentDrawers.push_back(currentTransparentDrawer);
			currentTransparentDrawer = nullptr;
		}
	}

	queuedCommandTransparent.clear();
	queuedCommandOpaque.clear();
}
