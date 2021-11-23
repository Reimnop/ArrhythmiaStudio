#pragma once
#include <string>

#include "EditorWindow.h"
#include "imgui/imgui.h"

class Timeline : public EditorWindow
{
public:
	~Timeline() override = default;

	std::string getTitle() override;
	void draw() override;
private:
	float startTime = 0.0f;
	float endTime = 12.0f;

	void drawTimeline();
	void drawObjectStrip(std::string& name, ImVec2 min, ImVec2 max, bool highlighted);
};
