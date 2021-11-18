#include "GameManager.h"

#include "../rendering/ImGuiController.h"
#include "../rendering/Renderer.h"
#include "VideoExporter.h"

#include <fstream>
#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_markdown.h>
#include <ShlObj.h>
#include <helper.h>
#include <logger.h>

#define WELCOME_MSG Welcome to PROJECT_NAME!

GameManager::GameManager(GLFWwindow* window)
{
	mainWindow = window;

	ImGuiController::onLayout.push_back(std::bind(&GameManager::onLayout, this));

	discordManager = new DiscordManager();
	docManager = new DocManager();
}

void GameManager::update() const
{
	discordManager->update();
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

std::string GameManager::timeToString(float time) const
{
	float secs = std::floor(time);

	int seconds = (int)secs % 60;
	int minutes = (int)secs / 60 % 60;
	int hours = (int)secs / 3600 % 60;

	std::string secondsStr = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
	std::string minutesStr = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
	std::string hoursStr = hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);

	return hoursStr + ":" + minutesStr + ":" + secondsStr;
}
