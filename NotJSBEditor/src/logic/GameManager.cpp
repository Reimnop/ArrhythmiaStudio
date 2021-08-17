#include "GameManager.h"

#include "../rendering/ImGuiController.h"
#include "../rendering/Renderer.h"
#include "DataManager.h"

#include <functional>
#include <bass/bass.h>

GameManager::GameManager(GLFWwindow* window)
{
	mainWindow = window;

	ImGuiController::onLayout.push_back(std::bind(&GameManager::onLayout, this));

	shapeManager = new ShapeManager();
	levelManager = new LevelManager();
}

void GameManager::update()
{
	levelManager->update();
}

void GameManager::onLayout()
{
	// Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				DataManager::inst->saveLevel();
			}

			if (ImGui::MenuItem("Open"))
			{
				DataManager::inst->openLevel();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Open viewport
	if (ImGui::Begin("Viewport"))
	{
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();

		float width, height;
		calculateViewportRect(contentRegion, &width, &height);

		Renderer::inst->viewportWidth = width;
		Renderer::inst->viewportHeight = height;

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		ImVec2 frameMin = ImVec2(cursorPos.x + (contentRegion.x - width) / 2.0f,
		                         cursorPos.y + (contentRegion.y - height) / 2.0f);
		ImVec2 frameMax = ImVec2(frameMin.x + width, frameMin.y + height);

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->AddImage((ImTextureID)Renderer::inst->getRenderTexture(), ImVec2(frameMin.x, frameMax.y),
		                   ImVec2(frameMax.x, frameMin.y));
		drawList->AddRect(frameMin, frameMax, ImGui::GetColorU32(ImGuiCol_Border), 0.0f, ImDrawFlags_None, 2.0f);
	}
	ImGui::End();
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
