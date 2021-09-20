#include "VideoExporter.h"

#include "AudioClip.h"
#include "LevelManager.h"
#include "logger.h"
#include "../rendering/Renderer.h"

VideoExporter::VideoExporter(int width, int height, int framerate, int startFrame, int endFrame, ffmpegcpp::VideoCodec* videoCodec, ffmpegcpp::AudioCodec* audioCodec)
{
	this->width = width;
	this->height = height;
	this->framerate = framerate;
	this->startFrame = startFrame;
	this->endFrame = endFrame;
	this->videoCodec = videoCodec;
	this->audioCodec = audioCodec;
}

VideoExporter::~VideoExporter()
{
	delete videoCodec;
	delete audioCodec;
}

void VideoExporter::exportToVideo(const std::filesystem::path& path) const
{
	Renderer* renderer = Renderer::inst;
	LevelManager* levelManager = LevelManager::inst;

	// Store those so we can restore later
	const int oldWidth = renderer->viewportWidth;
	const int oldHeight = renderer->viewportHeight;
	const float oldTime = levelManager->time;

	// Resize so we get good quality images
	renderer->viewportWidth = width;
	renderer->viewportHeight = height;

	renderer->resizeViewport();

	// Prepare video encoder
	ffmpegcpp::Muxer* muxer = new ffmpegcpp::Muxer(path.generic_string().c_str());

	ffmpegcpp::VideoEncoder* videoEncoder = new ffmpegcpp::VideoEncoder(videoCodec, muxer);

	// Prepare audio encoder
	const AudioClip* clip = LevelManager::inst->audioClip;
	const int samplesPerFrame = clip->frequency / framerate;

	ffmpegcpp::AudioEncoder* audioEncoder = new ffmpegcpp::AudioEncoder(audioCodec, muxer);

	// Prepare video stream
	ffmpegcpp::RawVideoDataSource* videoStream = new ffmpegcpp::RawVideoDataSource(renderer->viewportWidth, renderer->viewportHeight, AV_PIX_FMT_RGBA, AV_PIX_FMT_YUV420P, framerate, videoEncoder);

	// Prepare audio stream
	ffmpegcpp::RawAudioDataSource* audioStream = new ffmpegcpp::RawAudioDataSource(AV_SAMPLE_FMT_S16, clip->frequency, clip->channelsCount, samplesPerFrame, audioEncoder);

	// Make a buffer to render our textures into
	uint8_t* colorBuffer = new uint8_t[renderer->viewportWidth * renderer->viewportHeight * 4];

	// Rendering
	for (int i = startFrame; i < endFrame; i++)
	{
		Logger::info("Rendering frame " + std::to_string(i));

		// Update and render
		levelManager->updateLevel(i / (float)framerate);
		renderer->renderViewport();

		glBindTexture(GL_TEXTURE_2D, renderer->getRenderTexture());
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorBuffer);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Flip image in place
		for (int y = 0; y < renderer->viewportHeight / 2; y++)
		{
			const int y1 = y;
			const int y2 = renderer->viewportHeight - y - 1;

			// Swap
			for (int x = 0; x < renderer->viewportWidth; x++)
			{
				const int r_index_1 = (y1 * renderer->viewportWidth + x) * 4 + 0;
				const int g_index_1 = (y1 * renderer->viewportWidth + x) * 4 + 1;
				const int b_index_1 = (y1 * renderer->viewportWidth + x) * 4 + 2;
				const int a_index_1 = (y1 * renderer->viewportWidth + x) * 4 + 3;

				const int r_index_2 = (y2 * renderer->viewportWidth + x) * 4 + 0;
				const int g_index_2 = (y2 * renderer->viewportWidth + x) * 4 + 1;
				const int b_index_2 = (y2 * renderer->viewportWidth + x) * 4 + 2;
				const int a_index_2 = (y2 * renderer->viewportWidth + x) * 4 + 3;

				std::swap(colorBuffer[r_index_1], colorBuffer[r_index_2]);
				std::swap(colorBuffer[g_index_1], colorBuffer[g_index_2]);
				std::swap(colorBuffer[b_index_1], colorBuffer[b_index_2]);
				std::swap(colorBuffer[a_index_1], colorBuffer[a_index_2]);
			}
		}

		videoStream->WriteFrame(colorBuffer, renderer->viewportWidth * 4);
		audioStream->WriteData(&clip->samples[i * samplesPerFrame * clip->channelsCount], samplesPerFrame);
	}

	// Cleanup
	delete[] colorBuffer;

	videoStream->Close();
	audioStream->Close();
	muxer->Close();

	delete videoEncoder;
	delete videoStream;
	delete audioEncoder;
	delete audioStream;
	delete muxer;

	// Restore resolution
	renderer->viewportWidth = oldWidth;
	renderer->viewportHeight = oldHeight;

	renderer->resizeViewport();

	// Restore timeline pos
	levelManager->updateLevel(oldTime);
}
