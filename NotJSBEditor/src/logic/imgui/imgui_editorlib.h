#pragma once

#include <imgui/imgui.h>

namespace ImGui
{
	bool EditorStripInputPass(const char* label, const ImVec2& min, const ImVec2& max, bool selected, bool* pressed, bool* highlighted);
	void EditorStripVisualPass(const char* label, const ImVec2& min, const ImVec2& max, bool highlighted);

	bool IntersectAABB(const ImVec2& min, const ImVec2& max, const ImVec2& pos);
}
