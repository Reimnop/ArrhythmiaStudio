#include "Renderer.h"

Renderer::Renderer(GLFWwindow* window, Scene* scene) {
	mainWindow = window;
	mainScene = scene;

	camera = new Camera();
	imGuiController = new ImGuiController(window, "Assets/Inconsolata.ttf");
}

void Renderer::update() {
	imGuiController->update();
}

void Renderer::render() {
	int width, height;
	glfwGetWindowSize(mainWindow, &width, &height);

    float aspect = width / (float)height;
    glm::mat4 view, projection;
    camera->calculateViewProjection(aspect, &view, &projection);
    recursivelyRenderNodes(mainScene->rootNode, glm::mat4(1.0f), view, projection);

    for (OutputDrawData drawData : queuedDrawData)
    {
        if (drawData.vao != lastVertexArray)
        {
            glBindVertexArray(drawData.vao);
            lastVertexArray = drawData.vao;
        }

        if (drawData.shader != lastShader)
        {
            glUseProgram(drawData.shader);
            lastShader = drawData.shader;
        }

        // Bind all uniform buffers
        if (drawData.uniformBuffers)
        {
            for (int i = 0; i < drawData.uniformBuffersCount; i++)
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, i, drawData.uniformBuffers[i]);
            }
        }

        switch (drawData.commandType) {
        case DrawCommandType_DrawElements:
            DrawElementsCommand drawElementsCommand = *(DrawElementsCommand*)drawData.drawCommand;
            glDrawElements(GL_TRIANGLES, drawElementsCommand.count, GL_UNSIGNED_INT, (void*)drawElementsCommand.offset);
            break;
        }

        // Free the draw command
        delete drawData.drawCommand;
    }

    // Clean up
    queuedDrawData.clear();

    lastVertexArray = 0;
    lastShader = 0;

    glBindVertexArray(0);
    glUseProgram(0);

	// Render ImGui last
	imGuiController->renderImGui();
}

void Renderer::recursivelyRenderNodes(SceneNode* node, glm::mat4 parentTransform, glm::mat4 view, glm::mat4 projection) {
    glm::mat4 nodeTransform = node->transform->getLocalMatrix();
    glm::mat4 globalTransform = parentTransform * nodeTransform;

    if (node->renderer)
    {
        InputDrawData drawData = InputDrawData();
        drawData.model = globalTransform;
        drawData.view = view;
        drawData.projection = projection;
        drawData.modelViewProjection = projection * view * globalTransform;

        OutputDrawData output;
        if (node->renderer->render(drawData, &output))
        {
            queuedDrawData.push_back(output);
        }
    }

    for (SceneNode* child : node->children)
    {
        recursivelyRenderNodes(child, globalTransform, view, projection);
    }
}