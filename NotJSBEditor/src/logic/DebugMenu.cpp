#include "DebugMenu.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <string>
#include <stb/stb_image_write.h>
#include <logger.h>
#include <helper.h>
#include <fstream>

#include "../rendering/ImGuiController.h"
#include "../MainWindow.h"
#include "LevelManager.h"

#include <ffmpegcpp.h>

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

		if (ImGui::Button("Export to Frame Sequence"))
		{
			Renderer* renderer = Renderer::inst;
			LevelManager* levelManager = LevelManager::inst;

			// Store those so we can restore later
			const int oldWidth = renderer->viewportWidth;
			const int oldHeight = renderer->viewportHeight;
			const float oldTime = levelManager->time;

			// Resize so we get good quality images
			renderer->viewportWidth = 1920;
			renderer->viewportHeight = 1080;

			renderer->resizeViewport();

			// Prepare encoder
			ffmpegcpp::Muxer* muxer = new ffmpegcpp::Muxer("video.mp4");

			ffmpegcpp::VideoCodec* codec = new ffmpegcpp::VideoCodec("h264_nvenc");
			codec->SetQualityScale(22);

			ffmpegcpp::VideoEncoder* encoder = new ffmpegcpp::VideoEncoder(codec, muxer);

			// Prepare audio stream
			const AudioClip* clip = LevelManager::inst->audioClip;
			const int samplesPerFrame = clip->frequency / 60;

			ffmpegcpp::AudioCodec* audioCodec = new ffmpegcpp::AudioCodec(AV_CODEC_ID_VORBIS);
			ffmpegcpp::AudioEncoder* audioEncoder = new ffmpegcpp::AudioEncoder(audioCodec, muxer);

			ffmpegcpp::RawAudioDataSource* audioStream = new ffmpegcpp::RawAudioDataSource(AV_SAMPLE_FMT_S16, clip->frequency, 1, audioEncoder);

			// Convert to mono (I'm too dumb for stereo)
			const int newSamplesLength = clip->samplesCount / clip->channelsCount;
			int16_t* samples = new int16_t[newSamplesLength];

			for (int i = 0; i < newSamplesLength; i++)
			{
				int16_t newSample = 0;
				for (int j = 0; j < clip->channelsCount; j++)
				{
					newSample += clip->samples[i * clip->channelsCount + j] / clip->channelsCount;
				}

				samples[i] = newSample;
			}

			// Prepare frame stream
			ffmpegcpp::FrameSinkStream* videoStream = encoder->CreateStream();

			// Prepare video metadata
			ffmpegcpp::StreamData videoMetadata = ffmpegcpp::StreamData();
			videoMetadata.timeBase = { 1, 60 };
			videoMetadata.frameRate = { 60, 1 };
			videoMetadata.type = AVMEDIA_TYPE_VIDEO;

			AVFrame* frame = av_frame_alloc();

			frame->width = renderer->viewportWidth;
			frame->height = renderer->viewportHeight;
			frame->format = AV_PIX_FMT_YUV444P;

			av_frame_get_buffer(frame, 0);

			// Make a buffer to copy our rendered textures into
			uint8_t* colorBuffer = new uint8_t[renderer->viewportWidth * renderer->viewportHeight * 4];

			// Rendering
			for (int i = 0; i < 3000; i++)
			{
				Logger::info("Rendering frame " + std::to_string(i));

				// Update and render
				levelManager->updateLevel(i / 60.0f);
				renderer->renderViewport();

				glBindTexture(GL_TEXTURE_2D, renderer->getRenderTexture());
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorBuffer);
				glBindTexture(GL_TEXTURE_2D, 0);

				for (int y = 0; y < renderer->viewportHeight; y++)
				{
					for (int x = 0; x < renderer->viewportWidth; x++)
					{
						const int bufIndex = (renderer->viewportHeight - y - 1) * renderer->viewportWidth + x;
						const int index = y * renderer->viewportWidth + x;

						double Yd, Ud, Vd;
						YUVfromRGB(Yd, Ud, Vd, colorBuffer[bufIndex * 4 + 0], colorBuffer[bufIndex * 4 + 1], colorBuffer[bufIndex * 4 + 2]);

						frame->data[0][index] = Yd;
						frame->data[1][index] = Ud;
						frame->data[2][index] = Vd;
					}
				}

				videoStream->WriteFrame(frame, &videoMetadata);
				audioStream->WriteData(&samples[i * samplesPerFrame], samplesPerFrame);
			}

			av_frame_free(&frame);

			delete[] colorBuffer;
			delete[] samples;
			
			videoStream->Close();
			audioStream->Close();
			muxer->Close();

			// Restore resolution
			renderer->viewportWidth = oldWidth;
			renderer->viewportHeight = oldHeight;

			// Restore timeline pos
			levelManager->updateLevel(oldTime);

			renderer->resizeViewport();
		}

		ImGui::End();
	}
}
