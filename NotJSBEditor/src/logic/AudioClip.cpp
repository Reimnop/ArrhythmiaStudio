#include "AudioClip.h"

#include <filesystem>
#include <iostream>

AudioClip::AudioClip(std::filesystem::path path)
{
	sample = BASS_SampleLoad(false, path.c_str(), 0, 0, 1, 0);
	channel = BASS_SampleGetChannel(sample, 0);

	BASS_SAMPLE info;
	BASS_SampleGetInfo(sample, &info);

	samplesCount = info.length / sizeof(int16_t);
	channelsCount = info.chans;
	frequency = info.freq;

	samples = new int16_t[samplesCount];
	BASS_SampleGetData(sample, samples);
}

AudioClip::~AudioClip()
{
	stop();
	BASS_ChannelFree(channel);
	BASS_SampleFree(sample);

	delete[] samples;
}

void AudioClip::play() const
{
	BASS_ChannelPlay(channel, false);
}

void AudioClip::pause() const
{
	BASS_ChannelPause(channel);
}

void AudioClip::stop() const
{
	BASS_ChannelStop(channel);
}

bool AudioClip::isPlaying() const
{
	return BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING;
}

float AudioClip::getLength() const
{
	QWORD bytePos = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
	return (float)BASS_ChannelBytes2Seconds(channel, bytePos);
}

float AudioClip::getPosition() const
{
	QWORD bytePos = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
	return (float)BASS_ChannelBytes2Seconds(channel, bytePos);
}

float AudioClip::getSpeed() const
{
	float value;
	BASS_ChannelGetAttribute(channel, BASS_ATTRIB_FREQ, &value);
	return value / frequency;
}

void AudioClip::setSpeed(float value) const
{
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_FREQ, value * frequency);
}

void AudioClip::seek(float time) const
{
	QWORD bytePos = BASS_ChannelSeconds2Bytes(channel, time);
	BASS_ChannelSetPosition(channel, bytePos, BASS_POS_BYTE);
}
