#pragma once
#include "EditorWindow.h"
#include "imgui/imgui.h"

class Viewport : public EditorWindow
{
public:
	~Viewport() override = default;

	std::string getTitle() override;
	void draw() override;
private:
	void calculateViewportRect(ImVec2 size, float* width, float* height);
};
