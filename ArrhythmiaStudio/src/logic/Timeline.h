#pragma once

#include <string>
#include <glm/glm.hpp>

#include "../rendering/ComputeShader.h"
#include "../rendering/Texture2D.h"
#include "AudioClip.h"
#include "LevelObjectProperties.h"

class LevelManager;

class Timeline
{
public:
	static Timeline* inst;

	Timeline();
	void genBuffer(AudioClip* clip);
private:
	float instantiationOffset = 0.0f;

	float startTime;
	float endTime;

	int layer = 0;

	std::vector<LevelObjectProperties> undoDataOldState;
	bool stripsDraggingLastFrame = false;

	glm::ivec2 oldWaveformSize = glm::ivec2(0);

	uint32_t audioBuffer;
	ComputeShader* waveformShader;
	ComputeShader* fxaaShader;
	Texture2D* waveformTex;

	void onLayout();
	bool playButton(bool playing);
	std::string timeToString(float time) const;
};
