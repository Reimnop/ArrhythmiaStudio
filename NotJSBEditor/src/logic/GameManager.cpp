#include "GameManager.h"

GameManager::GameManager(GLFWwindow* window)
{
	mainWindow = window;

	levelManager = new LevelManager();

	ImGuiController::onLayout.push_back(std::bind(&GameManager::onLayout, this));
}

void GameManager::update()
{
	levelManager->update(MainWindow::inst->time);
}

void GameManager::onLayout()
{
	// Open viewport
	if (ImGui::Begin("Viewport"))
	{
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();

		float width, height;
		calculateViewportRect(contentRegion, &width, &height);

		Renderer::inst->viewportWidth = width;
		Renderer::inst->viewportHeight = height;

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		ImVec2 imageMin = ImVec2(cursorPos.x + (contentRegion.x - width) / 2.0f, cursorPos.y + (contentRegion.y - height) / 2.0f);
		ImVec2 imageMax = ImVec2(imageMin.x + width, imageMin.y + height);

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->AddImage((ImTextureID)Renderer::inst->getRenderTexture(), imageMin, imageMax);
		drawList->AddRect(imageMin, imageMax, ImGui::GetColorU32(ImGuiCol_Border), 0.0f, ImDrawFlags_None, 2.0f);

		ImGui::End();
	}
}

void GameManager::calculateViewportRect(ImVec2 size, float* width, float* height)
{
	const float aspect = 16.0f / 9.0f;

	if (size.x / size.y > aspect) // Landscape
	{
		*width = size.y * aspect;
		*height = size.y;
	}
	else // Portrait
	{
		*width = size.x;
		*height = size.x / aspect;
	}
}
