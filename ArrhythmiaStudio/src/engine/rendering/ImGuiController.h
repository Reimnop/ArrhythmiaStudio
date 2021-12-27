#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

class ImGuiController
{
public:
	static std::vector<std::function<void()>> onLayout;

	ImGuiController(GLFWwindow* window, const char* fontPath);
	~ImGuiController();

	void update();
	void renderImGui();
private:
	void setStyle();
};
