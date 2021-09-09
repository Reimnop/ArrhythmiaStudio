#include "GameManager.h"

#include "../rendering/ImGuiController.h"
#include "../rendering/Renderer.h"

#include <fstream>
#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <ShlObj.h>

GameManager::GameManager(GLFWwindow* window)
{
	mainWindow = window;

	ImGuiController::onLayout.push_back(std::bind(&GameManager::onLayout, this));

	shapeManager = new ShapeManager();
	dataManager = new DataManager();
	discordManager = new DiscordManager();
	levelManager = new LevelManager();
	debug = new DebugMenu();
}

void GameManager::update() const
{
	levelManager->update();
	discordManager->update();
}

void GameManager::onLayout()
{
	// Welcome popup to block the user from doing anything on the startup level
	ImGui::SetNextWindowSize(ImVec2(320.0f, 140.0f));
	if (ImGui::BeginPopupModal("Welcome", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Text("Welcome to Not JSB Editor!");
		ImGui::TextWrapped("Before continuing, please create a new level with File->New (Ctrl+N) or open a new level with File->Open (Ctrl+O).");

		if (ImGui::Button("OK"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Level creation popup
	if (ImGui::BeginPopupModal("New Level", nullptr,
	                           ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
	                           ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::SetNextItemWidth(318.0f);
		ImGui::InputText("Level name", &currentCreateInfo.levelName);

		ImGui::PushID(1);
		if (ImGui::Button("Browse"))
		{
			COMDLG_FILTERSPEC filter;
			filter.pszName = L"Ogg Vorbis file";
			filter.pszSpec = L"*.ogg";

			IFileDialog* fd;
			CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fd));

			DWORD dwFlags;
			fd->GetOptions(&dwFlags);
			fd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
			fd->SetFileTypes(1, &filter);
			fd->Show(NULL);

			IShellItem* psiResult;
			if (SUCCEEDED(fd->GetResult(&psiResult)))
			{
				PWSTR pszFilePath = NULL;
				psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				std::wstring ws(pszFilePath);
				currentCreateInfo.levelSong = std::string(ws.begin(), ws.end());

				CoTaskMemFree(pszFilePath);
			}

			fd->Release();
		}
		ImGui::SameLine();
		ImGui::InputText("Level song", &currentCreateInfo.levelSong);
		ImGui::PopID();

		ImGui::PushID(2);
		if (ImGui::Button("Browse"))
		{
			IFileDialog* fd;
			CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fd));

			DWORD dwFlags;
			fd->GetOptions(&dwFlags);
			fd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_PICKFOLDERS);
			fd->Show(NULL);

			IShellItem* psiResult;
			if (SUCCEEDED(fd->GetResult(&psiResult)))
			{
				PWSTR pszFilePath = NULL;
				psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				std::wstring ws(pszFilePath);
				currentCreateInfo.levelPath = std::string(ws.begin(), ws.end());

				CoTaskMemFree(pszFilePath);
			}

			fd->Release();
		}
		ImGui::SameLine();
		ImGui::InputText("Level path", &currentCreateInfo.levelPath);
		ImGui::PopID();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			dataManager->newLevel(currentCreateInfo);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	bool doOpenLevelPopup = false;

	// Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl+N"))
			{
				doOpenLevelPopup = true;
			}

			if (!dataManager->isStartupLevel)
			{
				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
					dataManager->saveLevel();
				}

				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
				{
					dataManager->saveLevel(true);
				}
			}

			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
				dataManager->openLevel();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Keyboard shortcuts handling
	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_N))
	{
		doOpenLevelPopup = true;
	}

	if (!dataManager->isStartupLevel)
	{
		if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) &&
			ImGui::IsKeyPressed(GLFW_KEY_S))
		{
			dataManager->saveLevel(true);
		}

		if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_S))
		{
			dataManager->saveLevel();
		}
	}

	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_O))
	{
		dataManager->openLevel();
	}

	if (doOpenLevelPopup)
	{
		currentCreateInfo = LevelCreateInfo();

		ImGui::OpenPopup("New Level");
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

		if (dataManager->isStartupLevel)
		{
			ImGui::SetCursorScreenPos(ImVec2(frameMin.x + 4.0f, frameMin.y + 4.0f));
			if (ImGui::BeginChild("warn-startup", ImVec2(320.0f, 120.0f)))
			{
				ImGui::TextWrapped(
					"Warning: All changes made in the startup level will not be saved. Please create a new level or open an existing level.");

				ImGui::EndChild();
			}
		}
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
