#include "GameManager.h"

#include "../engine/rendering/ImGuiController.h"
#include "../engine/rendering/Renderer.h"
#include "object_behaviours/NormalObjectBehaviour.h"
#include "imgui/imgui.h"
#include "imgui/imgui_markdown.h"
#include "editor_windows/Timeline.h"
#include "editor_windows/Viewport.h"
#include "helper.h"
#include "logger.h"

#include <fstream>
#include <functional>

#define WELCOME_MSG Welcome to PROJECT_NAME!

GameManager::GameManager(GLFWwindow* window)
{
	mainWindow = window;

	ImGuiController::onLayout.push_back(std::bind(&GameManager::onLayout, this));

	discordManager = new DiscordManager();
	docManager = new DocManager();

	editorWindows.push_back(new Viewport());
	editorWindows.push_back(new Timeline());
}

void GameManager::update()
{
	
}

void GameManager::onLayout()
{
	// Welcome popup
	ImGui::SetNextWindowSize(ImVec2(320.0f, 140.0f));
	if (ImGui::BeginPopupModal("Welcome", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Text(STRINGIFY(WELCOME_MSG));
		ImGui::TextWrapped("Before continuing, please create a new level with File->New (Ctrl+N) or open a new level with File->Open (Ctrl+O).");
		if (ImGui::Button("OK"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::SetNextWindowSize(ImVec2(320.0f, 100.0f));
	if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::TextWrapped("This software uses libraries from the FFmpeg project under the LGPLv2.1");
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	bool doAboutPopup = false;

	// Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
			{
				docManager->isOpen = true;
			}

			if (ImGui::MenuItem("About"))
			{
				doAboutPopup = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (doAboutPopup)
	{
		ImGui::OpenPopup("About");
	}

	// Open welcome popup
	if (!welcomeOpened)
	{
		ImGui::OpenPopup("Welcome");

		welcomeOpened = true;
	}

	for (EditorWindow* window : editorWindows)
	{
		window->draw();
	}
}