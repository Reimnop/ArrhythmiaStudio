#include "imgui_editorlib.h"

#include <imgui/imgui_internal.h>

#include "../GlobalConstants.h"

bool ImGui::EditorStripInputPass(const char* label, const ImVec2& min, const ImVec2& max, bool selected, bool* pressed, bool* highlighted)
{
	ImGuiWindow* window = GetCurrentWindow();

	const ImGuiID id = window->GetID(label);

	bool hovered, held;
	*pressed = ButtonBehavior(ImRect(min, max), id, &hovered, &held);
	*highlighted = hovered || held || selected;

	return held;
}

void ImGui::EditorStripVisualPass(const char* label, const ImVec2& min, const ImVec2& max, bool highlighted)
{
	ImGuiWindow* window = GetCurrentWindow();

	ImDrawList& drawList = *window->DrawList;

	const ImVec2 textSize = CalcTextSize(label);
	const ImVec2 textRectMin = ImVec2(min.x + EDITOR_STRIP_LEFT, min.y);
	const ImVec2 textRectMax = ImVec2(min.x + EDITOR_STRIP_LEFT + EDITOR_STRIP_RIGHT + textSize.x, max.y);

	drawList.PushClipRect(min, max, true);

	drawList.AddRectFilled(min, max, highlighted ? EDITOR_STRIP_ACTIVE_COL : EDITOR_STRIP_INACTIVE_COL);
	drawList.AddRectFilled(textRectMin, textRectMax, EDITOR_STRIP_ACTIVE_COL);
	drawList.AddText(ImVec2(textRectMin.x + EDITOR_STRIP_TEXT_LEFT_MARGIN, textRectMin.y), EDITOR_STRIP_INACTIVE_COL, label);

	drawList.PopClipRect();
}

bool ImGui::IntersectAABB(const ImVec2& min, const ImVec2& max, const ImVec2& pos)
{
	return pos.x > min.x && pos.x < max.x && pos.y > min.y && pos.y < max.y;
}

bool ImGui::IntersectAABB(const ImVec2& min1, const ImVec2& max1, const ImVec2& min2, const ImVec2& max2)
{
	return min1.x < max2.x && min1.y < max2.y && max1.x > min2.x && max2.y > min2.y;
}
