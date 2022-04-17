#include "VideoExporter.h"

#include "GameManager.h"
#include "Level.h"
#include "../engine/AudioClip.h"
#include "../engine/rendering/Renderer.h"

void VideoExporter::exportToVideo(path path) const
{
	Renderer& renderer = *Renderer::inst;
	Level& level = *GameManager::inst->level;

	// Store those so we can restore later
	const int oldWidth = renderer.viewportWidth;
	const int oldHeight = renderer.viewportHeight;
	const float oldTime = level.time;

	// Resize so we get good quality images
	renderer.viewportWidth = width;
	renderer.viewportHeight = height;

	renderer.resizeViewport();

	// Prepare video encoder
	ffmpegcpp::Muxer muxer(path.generic_string().c_str());

	ffmpegcpp::VideoEncoder videoEncoder((ffmpegcpp::VideoCodec*)&videoCodec, &muxer);

	// Prepare audio encoder
	const AudioClip& clip = *level.clip;
	const int samplesPerFrame = clip.frequency / framerate;

	ffmpegcpp::AudioEncoder audioEncoder((ffmpegcpp::AudioCodec*)&audioCodec, &muxer);

	// Prepare video stream
	ffmpegcpp::RawVideoDataSource videoStream(renderer.viewportWidth, renderer.viewportHeight, AV_PIX_FMT_RGBA, AV_PIX_FMT_YUV420P, framerate, &videoEncoder);

	// Prepare audio stream
	ffmpegcpp::RawAudioDataSource audioStream(AV_SAMPLE_FMT_S16, clip.frequency, clip.channelsCount, samplesPerFrame, &audioEncoder);

	// Make a buffer to render our textures into
	std::vector<uint8_t> colorBuffer(renderer.viewportWidth * renderer.viewportHeight * 4);

	// Rendering
	for (int i = startFrame; i < endFrame; i++)
	{
		LOG4CXX_INFO(logger, "Rendering frame " << i);

		// Update and render
		level.seek(i / (float)framerate);
		renderer.renderViewport();

		glBindTexture(GL_TEXTURE_2D, renderer.getRenderTexture());
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorBuffer.data());
		glBindTexture(GL_TEXTURE_2D, 0);

		// Flip image in place
		for (int y = 0; y < renderer.viewportHeight / 2; y++)
		{
			const int y1 = y;
			const int y2 = renderer.viewportHeight - y - 1;

			// Swap top pixels to bottom pixels
			for (int x = 0; x < renderer.viewportWidth; x++)
			{
				const int r_index_1 = (y1 * renderer.viewportWidth + x) * 4 + 0;
				const int g_index_1 = (y1 * renderer.viewportWidth + x) * 4 + 1;
				const int b_index_1 = (y1 * renderer.viewportWidth + x) * 4 + 2;
				const int a_index_1 = (y1 * renderer.viewportWidth + x) * 4 + 3;

				const int r_index_2 = (y2 * renderer.viewportWidth + x) * 4 + 0;
				const int g_index_2 = (y2 * renderer.viewportWidth + x) * 4 + 1;
				const int b_index_2 = (y2 * renderer.viewportWidth + x) * 4 + 2;
				const int a_index_2 = (y2 * renderer.viewportWidth + x) * 4 + 3;

				std::swap(colorBuffer[r_index_1], colorBuffer[r_index_2]);
				std::swap(colorBuffer[g_index_1], colorBuffer[g_index_2]);
				std::swap(colorBuffer[b_index_1], colorBuffer[b_index_2]);
				std::swap(colorBuffer[a_index_1], colorBuffer[a_index_2]);
			}
		}

		videoStream.WriteFrame(colorBuffer.data(), renderer.viewportWidth * 4);

        // So I don't get segfault
        uint64_t samplesLeft = clip.samplesCount - i * samplesPerFrame * clip.channelsCount;
        if (samplesLeft > 0) {
            audioStream.WriteData(&clip.samples[i * samplesPerFrame * clip.channelsCount], std::min(samplesPerFrame, (int)samplesLeft));
        }
	}

	videoStream.Close();
	audioStream.Close();
	muxer.Close();

	// Restore resolution
	renderer.viewportWidth = oldWidth;
	renderer.viewportHeight = oldHeight;

	renderer.resizeViewport();

	// Restore timeline pos
	level.seek(oldTime);
}
