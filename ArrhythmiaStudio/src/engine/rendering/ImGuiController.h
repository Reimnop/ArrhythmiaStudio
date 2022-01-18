#pragma once

#include <GLFW/glfw3.h>

#include "event.h"

class ImGuiController
{
public:
	static inline Event<> onLayout;

	ImGuiController(GLFWwindow* window, const char* fontPath);
	~ImGuiController();

	void update();
	void renderImGui();
private:
	void setStyle();
};
