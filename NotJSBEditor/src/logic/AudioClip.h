#pragma once

#include <bass/bass.h>

class AudioClip
{
public:
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
	HSTREAM handle;
};
