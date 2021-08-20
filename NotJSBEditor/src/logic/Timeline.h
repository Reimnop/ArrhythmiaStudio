#pragma once

#include <string>
#include <glm/glm.hpp>

#include "../rendering/ComputeShader.h"
#include "../rendering/Texture2D.h"
#include "AudioClip.h"

class LevelManager;

class Timeline
{
public:
	static Timeline* inst;

	Timeline();
	void genBuffer(AudioClip* clip);
private:
	float startTime;
	float endTime;

	glm::ivec2 oldWaveformSize = glm::ivec2(0);

	uint32_t audioBuffer;
	ComputeShader* waveformShader;
	Texture2D* waveformTex;

	void onLayout();
	bool playButton(bool playing);
	std::string timeToString(float time);
	
	float lerp(float a, float b, float t);
};
