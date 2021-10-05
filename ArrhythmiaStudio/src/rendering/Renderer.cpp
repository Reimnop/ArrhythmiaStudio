#include "Renderer.h"

#include "PostProcessing/Bloom.h"
#include "GlobalConstants.h"

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

	// Initialize batch resources
	glGenVertexArrays(1, &mdVAO);
	glBindVertexArray(mdVAO);

	glGenBuffers(1, &mdVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mdVBO);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &mdEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	lastVertexBufferSize = 0;
	lastIndexBufferSize = 0;

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

	std::sort(queuedDrawDataOpaque.begin(), queuedDrawDataOpaque.end(), opaqueComp);
	std::sort(queuedDrawDataTransparent.begin(), queuedDrawDataTransparent.end(), transparentComp);

	FramebufferStack::push(multisampleFramebuffer);

	glViewport(0, 0, viewportWidth, viewportHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	for (const OutputDrawData* drawData : queuedDrawDataOpaque)
	{
		queueCommand(drawData);

		delete drawData;
	}

	if (!commands.empty())
	{
		// Upload vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, mdVBO);

		const size_t vSize = sizeof(glm::vec3) * cmdVertexBuffer.size();
		if (vSize > lastVertexBufferSize)
		{
			glBufferData(GL_ARRAY_BUFFER, vSize, cmdVertexBuffer.data(), GL_DYNAMIC_DRAW);
			lastVertexBufferSize = vSize;
		}
		else
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, cmdVertexBuffer.data());
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Upload index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdEBO);

		const size_t iSize = sizeof(uint32_t) * cmdIndexBuffer.size();
		if (iSize > lastIndexBufferSize)
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, cmdIndexBuffer.data(), GL_DYNAMIC_DRAW);
			lastIndexBufferSize = iSize;
		}
		else
		{
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, iSize, cmdIndexBuffer.data());
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Draw commands
		glBindVertexArray(mdVAO);

		Material* material = commands.front().material;
		int drawCount = 0;
		for (int i = 0; i < commands.size() - 1; i++)
		{
			const DrawCommand& cmd = commands[i];

			cmdCounts.push_back(cmd.indicesCount);
			cmdIndices.push_back((void*)(sizeof(uint32_t) * cmd.indexOffset));
			cmdBaseVertices.push_back(cmd.vertexOffset);

			drawCount++;

			if (commands[i + 1].material != material)
			{
				const Shader* shader = material->getShader();
				if (lastShader != shader->getHandle())
				{
					glUseProgram(shader->getHandle());
					lastShader = shader->getHandle();
				}

				glBindBufferBase(GL_UNIFORM_BUFFER, 0, material->getUniformBuffer());

				glUniformMatrix4fv(0, 1, false, &viewProjection[0][0]);
				glUniform1f(1, 1.0f);

				glMultiDrawElementsBaseVertex(GL_TRIANGLES, cmdCounts.data(), GL_UNSIGNED_INT, cmdIndices.data(), drawCount, cmdBaseVertices.data());

				cmdCounts.clear();
				cmdIndices.clear();
				cmdBaseVertices.clear();

				drawCount = 0;

				material = commands[i + 1].material;
			}
		}

		const DrawCommand& cmd = commands.back();

		cmdCounts.push_back(cmd.indicesCount);
		cmdIndices.push_back((void*)(sizeof(uint32_t) * cmd.indexOffset));
		cmdBaseVertices.push_back(cmd.vertexOffset);

		drawCount++;

		const Shader* shader = material->getShader();
		if (lastShader != shader->getHandle())
		{
			glUseProgram(shader->getHandle());
			lastShader = shader->getHandle();
		}

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, material->getUniformBuffer());

		glUniformMatrix4fv(0, 1, false, &viewProjection[0][0]);
		glUniform1f(1, 1.0f);

		glMultiDrawElementsBaseVertex(GL_TRIANGLES, cmdCounts.data(), GL_UNSIGNED_INT, cmdIndices.data(), drawCount, cmdBaseVertices.data());

		cmdCounts.clear();
		cmdIndices.clear();
		cmdBaseVertices.clear();
	}

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (const OutputDrawData* drawData : queuedDrawDataTransparent)
	{
		if (!drawData->mesh || !drawData->material)
		{
			continue;
		}

		const Material* material = drawData->material;
		const Shader* shader = material->getShader();
		const Mesh* mesh = drawData->mesh;

		if (lastShader != shader->getHandle())
		{
			glUseProgram(shader->getHandle());
			lastShader = shader->getHandle();
		}

		if (lastVertexArray != mesh->getVao())
		{
			glBindVertexArray(mesh->getVao());
			lastVertexArray = mesh->getVao();
		}

		glm::mat4 mvp = viewProjection * drawData->transform;

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, material->getUniformBuffer());

		glUniformMatrix4fv(0, 1, false, &mvp[0][0]);
		glUniform1f(1, drawData->opacity);

		glDrawElements(GL_TRIANGLES, mesh->indicesCount, GL_UNSIGNED_INT, nullptr);

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

	commands.clear();
	cmdVertexBuffer.clear();
	cmdIndexBuffer.clear();

	lastVertexArray = 0;
	lastShader = 0;

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
		OutputDrawData* output;
		if (node->renderer->render(globalTransform, &output))
		{
			if (output->drawTransparent)
			{
				queuedDrawDataTransparent.push_back(output);
			}
			else
			{
				queuedDrawDataOpaque.push_back(output);
			}
		}
	}

	for (SceneNode* child : node->activeChildren)
	{
		recursivelyRenderNodes(child, globalTransform);
	}
}

void Renderer::queueCommand(const OutputDrawData* drawData)
{
	if (!drawData->mesh || !drawData->material)
	{
		return;
	}

	const Mesh* mesh = drawData->mesh;

	if (commands.empty() || 
		commands.back().material != drawData->material || 
		commands.back().verticesCount >= EST_MAX_BATCH_VERTICES || 
		commands.back().indicesCount >= EST_MAX_BATCH_INDICES)
	{
		DrawCommand newCmd = DrawCommand();

		newCmd.verticesCount = 0;
		newCmd.indicesCount = 0;

		if (commands.empty())
		{
			newCmd.vertexOffset = 0;
			newCmd.indexOffset = 0;
		}
		else
		{
			const DrawCommand& lastCmd = commands.back();
			newCmd.vertexOffset = lastCmd.vertexOffset + lastCmd.verticesCount;
			newCmd.indexOffset = lastCmd.indexOffset + lastCmd.indicesCount;
		}

		newCmd.material = drawData->material;

		commands.push_back(newCmd);
	}

	DrawCommand& cmd = commands.back();

	for (int i = 0; i < mesh->verticesCount; i++)
	{
		glm::vec3 aPos = mesh->vertices[i];
		glm::vec3 transformedPos = glm::vec3(drawData->transform * glm::vec4(aPos, 1.0f));

		cmdVertexBuffer.push_back(transformedPos);
	}

	for (int i = 0; i < mesh->indicesCount; i++)
	{
		cmdIndexBuffer.push_back(cmd.verticesCount + mesh->indices[i]);
	}

	cmd.verticesCount += mesh->verticesCount;
	cmd.indicesCount += mesh->indicesCount;
}
