#include "AudioClip.h"

#include <filesystem>
#include <iostream>

AudioClip::AudioClip(std::filesystem::path path)
{
	sample = BASS_SampleLoad(false, path.c_str(), 0, 0, 1, BASS_SAMPLE_FLOAT);
	channel = BASS_SampleGetChannel(sample, 0);

	BASS_SAMPLE info;
	BASS_SampleGetInfo(sample, &info);

	samplesCount = info.length / sizeof(float);
	channelsCount = info.chans;
	frequency = info.freq;

	samples = new float[samplesCount];
	BASS_SampleGetData(sample, samples);
}

AudioClip::~AudioClip()
{
	stop();
	BASS_ChannelFree(channel);
	BASS_SampleFree(sample);

	delete[] samples;
}

void AudioClip::play()
{
	BASS_ChannelPlay(channel, false);
}

void AudioClip::pause()
{
	BASS_ChannelPause(channel);
}

void AudioClip::stop()
{
	BASS_ChannelStop(channel);
}

bool AudioClip::isPlaying()
{
	return BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING;
}

float AudioClip::getLength()
{
	QWORD bytePos = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
	return (float)BASS_ChannelBytes2Seconds(channel, bytePos);
}

float AudioClip::getPosition()
{
	QWORD bytePos = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
	return (float)BASS_ChannelBytes2Seconds(channel, bytePos);
}

void AudioClip::seek(float time)
{
	QWORD bytePos = BASS_ChannelSeconds2Bytes(channel, time);
	BASS_ChannelSetPosition(channel, bytePos, BASS_POS_BYTE);
}
