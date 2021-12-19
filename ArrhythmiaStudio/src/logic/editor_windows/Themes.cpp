#include "Themes.h"
#include "../GameManager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "GLFW/glfw3.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui.h"

std::string Themes::getTitle()
{
	return "Themes";
}

void Themes::draw()
{
	GameManager* gameManager = GameManager::inst;
	Level* level = gameManager->level;

	if (ImGui::BeginChild("##color-sequences", ImVec2(0.0f, 120.0f), true))
	{
		// Deselect
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow) && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			level->selection.selectedColorSequence.reset();
		}

		for (size_t i = 0; i < gameManager->level->colorSequences.size(); i++)
		{
			ColorSequence& sequence = *gameManager->level->colorSequences[i];

			std::string title = "Color slot " + std::to_string(i);

			if (ImGui::Selectable(
				title.c_str(),
				level->selection.selectedColorSequence.has_value() ? 
				&level->selection.selectedColorSequence->get() == &sequence :
				false))
			{
				level->selection.selectedColorSequence = sequence;
			}
		}
	}
	ImGui::EndChild();

	if (level->selection.selectedColorSequence.has_value()) 
	{
		drawEditor();
	}
}

void Themes::drawEditor()
{
	GameManager* gameManager = GameManager::inst;
	Level* level = gameManager->level;

	if (beginKeyframeEditor())
	{
		assert(level->selection.selectedColorSequence.has_value());

		sequenceEdit(level->selection.selectedColorSequence.value(), "Color");

		endKeyframeEditor();
	}

	// Draw keyframe editor
	if (selectedKeyframe.has_value())
	{
		int index = selectedKeyframe.value().index;
		ColorSequence& sequence = *selectedKeyframe.value().sequence;

		ColorKeyframe* kf;
		if (isKeyframeTimeEditing())
		{
			assert(timeEditingKeyframe.has_value());

			ColorKeyframeTimeEditInfo& editInfo = timeEditingKeyframe.value();
			kf = &editInfo.keyframes[editInfo.index];

			updateKeyframeTimeEdit();
		}
		else
		{
			kf = &sequence.keyframes[index];
		}

		ImGui::DragFloat("Keyframe time", &kf->time, 0.1f);
		bool beginTimeEdit = ImGui::IsItemActivated();
		bool endTimeEdit = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::ColorEdit3("Keyframe value", &kf->color.r);

		std::string currentEaseName = Easing::getEaseName(kf->easing);
		if (ImGui::BeginCombo("Keyframe easing", currentEaseName.c_str()))
		{
			for (int i = 0; i < EaseType_Count; i++)
			{
				std::string easeName = Easing::getEaseName((EaseType)i);
				if (ImGui::Selectable(easeName.c_str(), kf->easing == i))
				{
					kf->easing = (EaseType)i;
				}
			}

			ImGui::EndCombo();
		}

		// Time edit defer
		if (beginTimeEdit)
		{
			beginKeyframeTimeEdit(index, sequence);
		}

		if (endTimeEdit)
		{
			endKeyframeTimeEdit();
		}
	}
}

bool Themes::beginKeyframeEditor()
{
	sequencesToDraw.clear();

	return true;
}

void Themes::sequenceEdit(ColorSequence& sequence, std::string label)
{
	sequencesToDraw.push_back(std::make_tuple(std::reference_wrapper(sequence), label));
}

void Themes::endKeyframeEditor()
{
	// Configurations
	constexpr float SEQUENCE_LABEL_PADDING = 8.0f;
	constexpr float SEQUENCE_HEIGHT = 20.0f;
	constexpr ImU32 SEQUENCE_PRIMARY_COL = 0xFF1F1F1F;
	constexpr ImU32 SEQUENCE_SECONDARY_COL = 0xFF2E2E2E;

	constexpr float TIME_POINTER_WIDTH = 18.0f;
	constexpr float TIME_POINTER_HEIGHT = 30.0f;
	constexpr float TIME_POINTER_TRI_HEIGHT = 8.0f;

	constexpr float KEYFRAME_SIZE = 15.0f;
	constexpr float HALF_KEYFRAME_SIZE = KEYFRAME_SIZE / 2.0f;
	constexpr ImU32 KEYFRAME_INACTIVE_COL = 0xFFBABABA;
	constexpr ImU32 KEYFRAME_ACTIVE_COL = 0xFF626262;

	// Data
	ImGuiWindow& window = *ImGui::GetCurrentWindow();
	ImGuiIO& io = ImGui::GetIO();
	ImGuiContext& context = *ImGui::GetCurrentContext();
	ImGuiStorage& storage = *ImGui::GetStateStorage();
	Level& level = *GameManager::inst->level;
	int sequenceCount = sequencesToDraw.size();

	// Drawing
	ImVec2 baseCoord = ImGui::GetCursorScreenPos();
	ImVec2 size = ImVec2(ImGui::GetContentRegionAvailWidth(), sequenceCount * SEQUENCE_HEIGHT + TIME_POINTER_HEIGHT);
	ImVec2 timePointerAreaSize = ImVec2(size.x, TIME_POINTER_HEIGHT);
	ImVec2 keyframeEditorSize = ImVec2(size.x, sequenceCount * SEQUENCE_HEIGHT);

	// Storage
	ImGuiID startTimeId = window.GetID("##kf-editor-st");
	ImGuiID endTimeId = window.GetID("##kf-editor-et");

	float startTime = storage.GetFloat(startTimeId, 0.0f);
	float endTime = storage.GetFloat(endTimeId, 1.0f);

	// Input handling
	ImRect pointerRect = ImRect(
		baseCoord,
		baseCoord + timePointerAreaSize);

	ImRect keyframeEditorRect = ImRect(
		baseCoord + ImVec2(0.0f, timePointerAreaSize.y),
		baseCoord + size);

	// Time pointer dragging action
	ImGuiID pointerID = window.GetID("##time-pointer");

	if (ImGui::IsWindowFocused() && context.MovingWindow != &window && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && pointerRect.Contains(io.MousePos))
	{
		ImGui::SetActiveID(pointerID, &window);
		ImGui::SetFocusID(pointerID, &window);
		ImGui::FocusWindow(&window);
	}

	if (context.ActiveId == pointerID && context.ActiveIdSource == ImGuiInputSource_Mouse && !io.MouseDown[0])
	{
		ImGui::ClearActiveID();
	}

	if (context.ActiveId == pointerID)
	{
		float newTime = startTime + (io.MousePos.x - baseCoord.x) / size.x * (endTime - startTime);
		newTime = std::clamp(newTime, 0.0f, level.levelLength);

		level.seek(newTime);
	}

	// Zoom and pan
	if (ImGui::IsWindowFocused() && keyframeEditorRect.Contains(io.MousePos))
	{
		float visibleLength = endTime - startTime;
		float currentPos = (startTime + endTime) * 0.5f;

		float zoom = visibleLength / level.levelLength;

		zoom -= io.MouseWheel * 0.05f;

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
		{
			currentPos -= io.MouseDelta.x / size.x * visibleLength;
		}

		float minZoom = 2.0f / level.levelLength;
		zoom = std::clamp(zoom, minZoom, 1.0f);

		float newVisibleLength = zoom * level.levelLength;

		float minPos = newVisibleLength * 0.5f;
		float maxPos = level.levelLength - minPos;

		currentPos = std::clamp(currentPos, minPos, maxPos);

		startTime = currentPos - newVisibleLength * 0.5f;
		endTime = currentPos + newVisibleLength * 0.5f;

		storage.SetFloat(startTimeId, startTime);
		storage.SetFloat(endTimeId, endTime);
	}

	// Drawing
	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	drawList.PushClipRect(baseCoord, baseCoord + size, true);
	{
		ImVec2 keyframeEditorBase = baseCoord + ImVec2(0.0f, timePointerAreaSize.y);

		// Draw background
		drawList.AddRectFilled(keyframeEditorBase, keyframeEditorBase + keyframeEditorSize, SEQUENCE_PRIMARY_COL);

		// Draw foreground
		for (int i = 1; i < sequenceCount; i += 2)
		{
			ImVec2 sequenceBase = keyframeEditorBase + ImVec2(0.0f, SEQUENCE_HEIGHT * i);
			drawList.AddRectFilled(sequenceBase, sequenceBase + ImVec2(size.x, SEQUENCE_HEIGHT), SEQUENCE_SECONDARY_COL);
		}

		{
			std::optional<ColorKeyframeInfo> hoveredKeyframe;
			std::optional<ColorKeyframeInfo> lastClickedKeyframe;

			// Input pass
			for (int i = 0; i < sequenceCount; i++)
			{
				ImVec2 baseSequenceCoord = keyframeEditorBase + ImVec2(0.0f, i * SEQUENCE_HEIGHT);
				ColorSequence& sequence = std::get<0>(sequencesToDraw[i]);

				for (int j = 0; j < sequence.keyframes.size(); j++)
				{
					float kfPos = (sequence.keyframes[j].time - startTime) / (endTime - startTime) * size.x;
					if (kfPos < -HALF_KEYFRAME_SIZE || kfPos > size.x + HALF_KEYFRAME_SIZE)
					{
						continue;
					}

					ImRect kfRect = ImRect(
						baseSequenceCoord + ImVec2(kfPos - HALF_KEYFRAME_SIZE, 0.0f),
						baseSequenceCoord + ImVec2(kfPos + HALF_KEYFRAME_SIZE, SEQUENCE_HEIGHT));

					if (kfRect.Contains(io.MousePos))
					{
						ColorKeyframeInfo kfInfo;
						kfInfo.index = j;
						kfInfo.sequence = &sequence;

						hoveredKeyframe = kfInfo;
					}

					if (kfRect.Contains(io.MouseClickedPos[0]))
					{
						ColorKeyframeInfo kfInfo;
						kfInfo.index = j;
						kfInfo.sequence = &sequence;

						lastClickedKeyframe = kfInfo;
					}
				}
			}

			// Draw pass
			for (int i = 0; i < sequenceCount; i++)
			{
				ImVec2 baseSequenceCoord = keyframeEditorBase + ImVec2(0.0f, i * SEQUENCE_HEIGHT);
				ColorSequence& sequence = std::get<0>(sequencesToDraw[i]);
				std::string label = std::get<1>(sequencesToDraw[i]);

				drawList.AddText(
					baseSequenceCoord + ImVec2(SEQUENCE_LABEL_PADDING, 0.0f),
					ImGui::GetColorU32(ImGuiCol_Text),
					label.c_str());

				for (int j = 0; j < sequence.keyframes.size(); j++)
				{
					float kfPos = (sequence.keyframes[j].time - startTime) / (endTime - startTime) * size.x;
					if (kfPos < -HALF_KEYFRAME_SIZE || kfPos > size.x + HALF_KEYFRAME_SIZE)
					{
						continue;
					}

					bool isHighlighted = false;
					if (hoveredKeyframe.has_value() && hoveredKeyframe->index == j && hoveredKeyframe->sequence == &sequence)
					{
						isHighlighted = true;
					}

					if (isKeyframeTimeEditing())
					{
						assert(timeEditingKeyframe.has_value());

						if (
							timeEditingKeyframe->keyframes[timeEditingKeyframe->index] == sequence.keyframes[j] &&
							timeEditingKeyframe->sequence == &sequence)
						{
							isHighlighted = true;
						}
					}
					else
					{
						if (selectedKeyframe.has_value() && selectedKeyframe->index == j && selectedKeyframe->sequence == &sequence)
						{
							isHighlighted = true;
						}
					}

					ImVec2 kfCenter = baseSequenceCoord + ImVec2(kfPos, SEQUENCE_HEIGHT / 2.0f);
					drawList.AddQuadFilled(
						kfCenter + ImVec2(0.0f, HALF_KEYFRAME_SIZE),
						kfCenter + ImVec2(-HALF_KEYFRAME_SIZE, 0.0f),
						kfCenter + ImVec2(0.0f, -HALF_KEYFRAME_SIZE),
						kfCenter + ImVec2(HALF_KEYFRAME_SIZE, 0.0f),
						isHighlighted ? KEYFRAME_ACTIVE_COL : KEYFRAME_INACTIVE_COL);
				}
			}

			if (ImGui::IsWindowFocused() && keyframeEditorRect.Contains(io.MousePos) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				selectedKeyframe = hoveredKeyframe;
			}

			// Create new keyframe
			if (ImGui::IsWindowFocused() && keyframeEditorRect.Contains(io.MousePos) && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ColorSequence& sequence = std::get<0>(sequencesToDraw[(io.MousePos.y - keyframeEditorBase.y) / SEQUENCE_HEIGHT]);
				float time = startTime + ((io.MousePos.x - keyframeEditorBase.x) / size.x) * (endTime - startTime);
				ColorKeyframe kf;
				kf.time = time;
				kf.color = Color(1.0f, 1.0f, 1.0f);
				kf.easing = EaseType_Linear;

				sequence.insertKeyframe(kf);
			}

			// Keyframe delete
			if (ImGui::IsWindowFocused() && selectedKeyframe.has_value() && ImGui::IsKeyDown(GLFW_KEY_DELETE))
			{
				ColorKeyframeInfo& kfInfo = selectedKeyframe.value();
				// We don't do it via the eraseKeyframe method because we already know the index
				// and removing any item from a sorted vector results in a sorted vector.
				kfInfo.sequence->keyframes.erase(kfInfo.sequence->keyframes.begin() + kfInfo.index);
				selectedKeyframe.reset();
			}

			// Keyframe dragging action
			ImGuiID keyframeDragID = window.GetID("##keyframe-drag");

			if (ImGui::IsWindowFocused() && context.MovingWindow != &window && lastClickedKeyframe.has_value() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				ImGui::SetActiveID(keyframeDragID, &window);
				ImGui::SetFocusID(keyframeDragID, &window);
				ImGui::FocusWindow(&window);

				selectedKeyframe = lastClickedKeyframe;
				ColorKeyframeInfo& kfInfo = selectedKeyframe.value();

				beginKeyframeTimeEdit(kfInfo.index, *kfInfo.sequence);
			}

			if (context.ActiveId == keyframeDragID && context.ActiveIdSource == ImGuiInputSource_Mouse && !io.MouseDown[0])
			{
				ImGui::ClearActiveID();

				endKeyframeTimeEdit();
			}

			if (context.ActiveId == keyframeDragID)
			{
				assert(isKeyframeTimeEditing());
				assert(timeEditingKeyframe.has_value());

				ColorKeyframeTimeEditInfo& editInfo = timeEditingKeyframe.value();
				ColorKeyframe& kf = editInfo.keyframes[editInfo.index];

				float timeDelta = (io.MouseDelta.x / size.x) * (endTime - startTime);
				timeDelta = std::clamp(timeDelta, -kf.time, level.levelLength - kf.time);

				kf.time += timeDelta;

				updateKeyframeTimeEdit();
			}
		}

		drawList.PopClipRect();
	}

	// Draw borders
	drawList.AddRect(baseCoord, baseCoord + size, ImGui::GetColorU32(ImGuiCol_Border));
	drawList.AddLine(
		baseCoord + ImVec2(0.0f, TIME_POINTER_HEIGHT),
		baseCoord + ImVec2(size.x, TIME_POINTER_HEIGHT),
		ImGui::GetColorU32(ImGuiCol_Border));

	// Draw time pointer last
	drawList.PushClipRect(baseCoord, baseCoord + size, true);
	{
		float pointerPos = (level.time - startTime) / (endTime - startTime) * size.x;

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

	ImGui::ItemSize(size);
}

void Themes::beginKeyframeTimeEdit(int index, ColorSequence& sequence)
{
	ColorKeyframeTimeEditInfo editInfo;
	editInfo.index = index;
	editInfo.sequence = &sequence;
	editInfo.keyframes = sequence.keyframes;

	timeEditingKeyframe = editInfo;
}

void Themes::endKeyframeTimeEdit()
{
	assert(timeEditingKeyframe.has_value());

	ColorKeyframeTimeEditInfo& info = timeEditingKeyframe.value();
	ColorKeyframe kf = info.keyframes[info.index];

	auto it = std::find(info.sequence->keyframes.begin(), info.sequence->keyframes.end(), kf);

	ColorKeyframeInfo newInfo;
	newInfo.index = it - info.sequence->keyframes.begin();
	newInfo.sequence = info.sequence;

	selectedKeyframe = newInfo;

	timeEditingKeyframe.reset();
}

void Themes::updateKeyframeTimeEdit()
{
	assert(timeEditingKeyframe.has_value());

	ColorKeyframeTimeEditInfo& editInfo = timeEditingKeyframe.value();
	editInfo.sequence->loadKeyframes(editInfo.keyframes);
}

bool Themes::isKeyframeTimeEditing()
{
	return timeEditingKeyframe.has_value();
}

