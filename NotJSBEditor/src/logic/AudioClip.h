#pragma once

#include <cstdint>
#include <filesystem>
#include <bass/bass.h>

class AudioClip
{
public:
	float* samples;
	uint64_t samplesCount;
	uint32_t channelsCount;
	uint32_t frequency;

	AudioClip(std::filesystem::path path);
	~AudioClip();

	void play() const;
	void pause() const;
	void stop() const;

	bool isPlaying() const;

	float getLength() const;
	float getPosition() const;
	float getSpeed() const;

	void setSpeed(float value) const;

	void seek(float time) const;
private:
	HSAMPLE sample;
	HCHANNEL channel;
};
