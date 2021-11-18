#pragma once

#include <imgui/imgui.h>

namespace ImGui
{
	bool IntersectAABB(const ImVec2& min, const ImVec2& max, const ImVec2& pos);
	bool IntersectAABB(const ImVec2& min1, const ImVec2& max1, const ImVec2& min2, const ImVec2& max2);
}
