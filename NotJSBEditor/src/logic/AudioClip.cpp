#include "AudioClip.h"

AudioClip::AudioClip(const char* path)
{
	handle = BASS_StreamCreateFile(false, path, 0, 0, BASS_SAMPLE_FLOAT);
}

AudioClip::~AudioClip()
{
	stop();
	BASS_StreamFree(handle);
}

void AudioClip::play()
{
	BASS_ChannelPlay(handle, false);
}

void AudioClip::pause()
{
	BASS_ChannelPause(handle);
}

void AudioClip::stop()
{
	BASS_ChannelStop(handle);
}

bool AudioClip::isPlaying()
{
	return BASS_ChannelIsActive(handle) == BASS_ACTIVE_PLAYING;
}

float AudioClip::getLength()
{
	QWORD bytePos = BASS_ChannelGetLength(handle, BASS_POS_BYTE);
	return (float)BASS_ChannelBytes2Seconds(handle, bytePos);
}

float AudioClip::getPosition()
{
	QWORD bytePos = BASS_ChannelGetPosition(handle, BASS_POS_BYTE);
	return (float)BASS_ChannelBytes2Seconds(handle, bytePos);
}

void AudioClip::seek(float time)
{
	QWORD bytePos = BASS_ChannelSeconds2Bytes(handle, time);
	BASS_ChannelSetPosition(handle, bytePos, BASS_POS_BYTE);
}
