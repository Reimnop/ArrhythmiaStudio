#pragma once

#include <cstdint>
#include <bass/bass.h>

class AudioClip
{
public:
	float* samples;
	uint64_t samplesCount;
	uint32_t channelsCount;
	uint32_t frequency;

	AudioClip(const char* path);
	~AudioClip();

	void play();
	void pause();
	void stop();

	bool isPlaying();

	float getLength();
	float getPosition();

	void seek(float time);
private:
	HSAMPLE sample;
	HCHANNEL channel;
};
