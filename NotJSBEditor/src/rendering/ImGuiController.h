#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
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
