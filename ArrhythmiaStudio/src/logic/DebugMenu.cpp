#include "DebugMenu.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <string>
#include <helper.h>

#include "../rendering/ImGuiController.h"
#include "../MainWindow.h"
#include "LevelManager.h"

DebugMenu::DebugMenu()
{
	ImGuiController::onLayout.push_back(std::bind(&DebugMenu::onLayout, this));
}

void DebugMenu::onLayout()
{
	const LevelManager* levelManager = LevelManager::inst;
	lastTime += MainWindow::inst->deltaTime;
	framerate++;

	if (lastTime > 1.0f)
	{
		lastFramerate = framerate;
		framerate = 0;

		lastTime -= std::floor(lastTime);
	}

	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyDown(GLFW_KEY_LEFT_ALT) && ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) && ImGui::IsKeyPressed(GLFW_KEY_D))
	{
		open = true;
	}

	if (open && ImGui::Begin("Debug", &open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
	{
		const std::string framerateStr = "Framerate: " + std::to_string(lastFramerate);
		ImGui::Text(framerateStr.c_str());

		const std::string frametimeStr = "Frametime: " + std::to_string_with_precision(MainWindow::inst->deltaTime * 1000.0f, 2) + "ms";
		ImGui::Text(frametimeStr.c_str());

		const std::string objectsCount = "Objects count: " + std::to_string(levelManager->level->levelObjects.size());
		ImGui::Text(objectsCount.c_str());

		ImGui::SliderInt("Swap Interval", &interval, 0, 4);
		if (ImGui::IsItemEdited())
		{
			glfwSwapInterval(interval);
		}

		ImGui::End();
	}
}
