#pragma once

#include "ffmpeg.h"

#include "std.h"

namespace ffmpegcpp
{
	class FFmpegException : std::exception
	{

	public:

		FFmpegException(std::string error);

		FFmpegException(std::string error, int returnValue);

        const char* what() const noexcept override;
	private:
        std::string errorStr;

		char error[AV_ERROR_MAX_STRING_SIZE];
	};
}