#pragma once

#include <filesystem>

#include "ffmpegcpp.h"
#include "log4cxx/logger.h"

using namespace std::filesystem;

class VideoExporter
{
public:
	VideoExporter(int width, int height, int framerate, int startFrame, int endFrame, const char* videoCodec, const char* audioCodec) :
		width(width),
		height(height),
		framerate(framerate),
		startFrame(startFrame),
		endFrame(endFrame),
		videoCodec(videoCodec),
		audioCodec(audioCodec)
	{
	}
	
	void exportToVideo(path path) const;
private:
	static inline log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("VideoExporter");

	int width;
	int height;
	int framerate;
	int startFrame;
	int endFrame;
	ffmpegcpp::VideoCodec videoCodec;
	ffmpegcpp::AudioCodec audioCodec;
};
