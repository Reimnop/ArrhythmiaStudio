#include "GameManager.h"

#include "../rendering/ImGuiController.h"
#include "../rendering/Renderer.h"
#include "VideoExporter.h"

#include <fstream>
#include <functional>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <ShlObj.h>
#include <helper.h>
#include <logger.h>

#define WELCOME_MSG Welcome to PROJECT_NAME!

GameManager::GameManager(GLFWwindow* window)
{
	mainWindow = window;

	ImGuiController::onLayout.push_back(std::bind(&GameManager::onLayout, this));

	shapeManager = new ShapeManager();
	dataManager = new DataManager();
	discordManager = new DiscordManager();
	undoRedoManager = new UndoRedoManager();
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

	// Level creation popup
	if (ImGui::BeginPopupModal("New Level", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
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

	if (ImGui::BeginPopupModal("Export Configuration", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		if (ImGui::Button("Browse"))
		{
			COMDLG_FILTERSPEC filter;
			filter.pszName = L"MKV file";
			filter.pszSpec = L"*.mkv";

			IFileDialog* fd;
			CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fd));

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
				videoPath = std::string(ws.begin(), ws.end());

				CoTaskMemFree(pszFilePath);
			}

			fd->Release();
		}
		ImGui::SameLine();
		ImGui::InputText("Video path", &videoPath);

		ImGui::InputInt("Width", &videoWidth);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45);
		ImGui::InputInt("Height", &videoHeight);

		ImGui::InputInt("Framerate", &videoFramerate);

		ImGui::InputInt("Start frame", &videoStartFrame);
		ImGui::SameLine();
		ImGui::InputInt("End frame", &videoEndFrame);

		if (ImGui::Button("Render"))
		{
			const VideoExporter* exporter = new VideoExporter(videoWidth, videoHeight, videoFramerate, videoStartFrame, videoEndFrame, new ffmpegcpp::VideoCodec(AV_CODEC_ID_VP9), new ffmpegcpp::AudioCodec(AV_CODEC_ID_AAC));
			exporter->exportToVideo(videoPath);

			delete exporter;

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
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

	bool doOpenLevelPopup = false;
	bool doExportVideoPopup = false;
	bool doAboutPopup = false;

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

			if (ImGui::MenuItem("Export to Video"))
			{
				doExportVideoPopup = true;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z"))
			{
				undoRedoManager->undo();
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y"))
			{
				undoRedoManager->redo();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				doAboutPopup = true;
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

	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_Z))
	{
		undoRedoManager->undo();
	}

	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_Y))
	{
		undoRedoManager->redo();
	}

	if (doOpenLevelPopup)
	{
		currentCreateInfo = LevelCreateInfo();

		ImGui::OpenPopup("New Level");
	}

	if (doExportVideoPopup)
	{
		ImGui::OpenPopup("Export Configuration");
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
