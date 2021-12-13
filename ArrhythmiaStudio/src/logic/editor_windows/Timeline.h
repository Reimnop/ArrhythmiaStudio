#pragma once
#include <string>

#include "EditorWindow.h"
#include "imgui/imgui.h"

#include "../../engine/rendering/ComputeShader.h"
#include "../../engine/rendering/Texture2D.h"

class Timeline : public EditorWindow
{
public:
	Timeline();
	~Timeline() override;

	std::string getTitle() override;
	void draw() override;
private:
	static inline const float WAVEFORM_FREQ = 50.0f;

	ImVec2 timelineSize;
	ImVec2 oldTimelineSize;

	float startTime = 0.0f;
	float endTime = 12.0f;

	uint32_t audioBuffer;
	ComputeShader* waveformShader;
	Texture2D* waveformTex;

	void genBuffer();
	void drawTimeline();
	void drawObjectStrip(std::string& name, ImVec2 min, ImVec2 max, bool highlighted);

	bool playButton(bool playing);
};
