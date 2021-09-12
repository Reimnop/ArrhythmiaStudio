#include "DebugMenu.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <string>
#include <helper.h>
#include <ShlObj.h>

#include "../rendering/ImGuiController.h"
#include "../MainWindow.h"
#include "LevelManager.h"
#include "VideoExporter.h"

DebugMenu::DebugMenu()
{
	ImGuiController::onLayout.push_back(std::bind(&DebugMenu::onLayout, this));
}

void YUVfromRGB(double& Y, double& U, double& V, const double R, const double G, const double B)
{
	Y = 0.257 * R + 0.504 * G + 0.098 * B + 16;
	U = -0.148 * R - 0.291 * G + 0.439 * B + 128;
	V = 0.439 * R - 0.368 * G - 0.071 * B + 128;
}

void DebugMenu::onLayout()
{
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

		ImGui::SliderInt("Swap Interval", &interval, 0, 4);
		if (ImGui::IsItemEdited())
		{
			glfwSwapInterval(interval);
		}

		if (ImGui::BeginPopupModal("Export Configuration", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::Button("Browse"))
			{
				COMDLG_FILTERSPEC filter;
				filter.pszName = L"MP4 file";
				filter.pszSpec = L"*.mp4";

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
				const VideoExporter* exporter = new VideoExporter(videoWidth, videoHeight, videoFramerate, videoStartFrame, videoEndFrame, new ffmpegcpp::VideoCodec("h264_nvenc"), new ffmpegcpp::AudioCodec(AV_CODEC_ID_AAC));
				exporter->exportToVideo(videoPath);

				delete exporter;
			}

			ImGui::EndPopup();
		}

		if (ImGui::Button("Export to Video"))
		{
			ImGui::OpenPopup("Export Configuration");
		}

		ImGui::End();
	}
}
