#include "FFmpegException.h"

using namespace std;

namespace ffmpegcpp
{
	FFmpegException::FFmpegException(string error)
	{
        this->errorStr = error;
	}

	FFmpegException::FFmpegException(string error, int returnValue)
	{
        this->errorStr = error + ": " + av_make_error_string(this->error, AV_ERROR_MAX_STRING_SIZE, returnValue);
	}

    const char* FFmpegException::what() const noexcept
    {
        return this->errorStr.c_str();
    }
}