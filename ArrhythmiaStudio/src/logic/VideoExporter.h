#pragma once

#include <filesystem>
#include <ffmpegcpp.h>

class VideoExporter
{
public:
	VideoExporter(int width, int height, int framerate, int startFrame, int endFrame, ffmpegcpp::VideoCodec* videoCodec, ffmpegcpp::AudioCodec* audioCodec);
	~VideoExporter();
	
	void exportToVideo(std::filesystem::path path) const;
private:
	int width;
	int height;
	int framerate;
	int startFrame;
	int endFrame;
	ffmpegcpp::VideoCodec* videoCodec;
	ffmpegcpp::AudioCodec* audioCodec;
};
