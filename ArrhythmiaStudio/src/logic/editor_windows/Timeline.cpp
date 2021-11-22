#include "Timeline.h"
#include "imgui/imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

void Timeline::draw()
{
	if (ImGui::Begin("Timeline"))
	{
		drawTimeline();
	}
	ImGui::End();
}

void Timeline::drawTimeline()
{
	// Configurations
	constexpr int BIN_COUNT = 15;
	constexpr float BIN_HEIGHT = 20.0f;
	constexpr ImU32 BIN_PRIMARY_COL = 0xFF1F1F1F;
	constexpr ImU32 BIN_SECONDARY_COL = 0xFF2E2E2E;

	constexpr float TIME_POINTER_WIDTH = 18.0f;
	constexpr float TIME_POINTER_HEIGHT = 30.0f;
	constexpr float TIME_POINTER_TRI_HEIGHT = 8.0f;

	// Drawing
	ImVec2 baseCoord = ImGui::GetCursorScreenPos();
	ImVec2 size = ImVec2(ImGui::GetContentRegionAvailWidth(), BIN_COUNT * BIN_HEIGHT + TIME_POINTER_HEIGHT);
	ImVec2 timePointerAreaSize = ImVec2(size.x, TIME_POINTER_HEIGHT);

	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	drawList.PushClipRect(baseCoord, baseCoord + size, true);
	{
		ImVec2 objectEditorBase = baseCoord + ImVec2(0.0f, timePointerAreaSize.y);
		ImVec2 objectEditorSize = ImVec2(size.x, BIN_COUNT * BIN_HEIGHT);

		// Draw background
		drawList.AddRectFilled(objectEditorBase, objectEditorBase + objectEditorSize, BIN_PRIMARY_COL);

		// Draw foreground
		for (int i = 1; i < BIN_COUNT; i += 2)
		{
			ImVec2 binBase = objectEditorBase + ImVec2(0.0f, BIN_HEIGHT * i);
			drawList.AddRectFilled(binBase, binBase + ImVec2(size.x, BIN_HEIGHT), BIN_SECONDARY_COL);
		}

		// Draw object strips (unimplemented)


		drawList.PopClipRect();
	}

	// Draw borders
	drawList.AddRect(baseCoord,baseCoord + size,ImGui::GetColorU32(ImGuiCol_Border));
	drawList.AddLine(
		baseCoord + ImVec2(0.0f, TIME_POINTER_HEIGHT),
		baseCoord + ImVec2(size.x, TIME_POINTER_HEIGHT),
		ImGui::GetColorU32(ImGuiCol_Border));

	// Draw time pointer last
	drawList.PushClipRect(baseCoord, baseCoord + size, true);
	{
		float pointerPos = 240.0f;
		drawList.AddLine(
			baseCoord + ImVec2(pointerPos, 0.0f),
			baseCoord + ImVec2(pointerPos, size.y),
			ImGui::GetColorU32(ImGuiCol_Border));

		drawList.AddRectFilled(
			baseCoord + ImVec2(pointerPos - TIME_POINTER_WIDTH * 0.5f, 0.0f),
			baseCoord + ImVec2(pointerPos + TIME_POINTER_WIDTH * 0.5f, TIME_POINTER_HEIGHT - TIME_POINTER_TRI_HEIGHT),
			ImGui::GetColorU32(ImGuiCol_Border));

		drawList.AddTriangleFilled(
			baseCoord + ImVec2(pointerPos - TIME_POINTER_WIDTH * 0.5f, TIME_POINTER_HEIGHT - TIME_POINTER_TRI_HEIGHT),
			baseCoord + ImVec2(pointerPos, TIME_POINTER_HEIGHT),
			baseCoord + ImVec2(pointerPos + TIME_POINTER_WIDTH * 0.5f, TIME_POINTER_HEIGHT - TIME_POINTER_TRI_HEIGHT),
			ImGui::GetColorU32(ImGuiCol_Border));

		drawList.PopClipRect();
	}
}
