#include "imgui_editorlib.h"

#include <imgui/imgui_internal.h>

bool ImGui::IntersectAABB(const ImVec2& min, const ImVec2& max, const ImVec2& pos)
{
	return pos.x > min.x && pos.x < max.x && pos.y > min.y && pos.y < max.y;
}

bool ImGui::IntersectAABB(const ImVec2& min1, const ImVec2& max1, const ImVec2& min2, const ImVec2& max2)
{
	return min1.x < max2.x && min1.y < max2.y && max1.x > min2.x && max2.y > min2.y;
}
