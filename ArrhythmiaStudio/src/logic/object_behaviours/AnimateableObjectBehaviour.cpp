#include "AnimateableObjectBehaviour.h"
#include "../Level.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

AnimateableObjectBehaviour::AnimateableObjectBehaviour(LevelObject* baseObject) : LevelObjectBehaviour(baseObject)
{
	Keyframe kf0 = Keyframe(0.0f, 0.0f, EaseType_Linear);
	Keyframe kf1 = Keyframe(0.0f, 1.0f, EaseType_Linear);
	positionX = Sequence(1, &kf0);
	positionY = Sequence(1, &kf0);
	scaleX = Sequence(1, &kf1);
	scaleY = Sequence(1, &kf1);
	rotation = Sequence(1, &kf0);
}

void AnimateableObjectBehaviour::update(float time)
{
	float t = time - baseObject->startTime;

	Transform& transform = *baseObject->node->transform;
	transform.position.x = positionX.update(t);
	transform.position.y = positionY.update(t);
	transform.scale.x = scaleX.update(t);
	transform.scale.y = scaleY.update(t);
	transform.rotation = angleAxis(rotation.update(t) / 180.0f * PI, glm::vec3(0.0f, 0.0f, -1.0f));
}

void AnimateableObjectBehaviour::readJson(json& j)
{
	positionX.fromJson(j["px"]);
	positionY.fromJson(j["py"]);
	scaleX.fromJson(j["sx"]);
	scaleY.fromJson(j["sy"]);
	rotation.fromJson(j["ro"]);
}

void AnimateableObjectBehaviour::writeJson(json& j)
{
	j["px"] = positionX.toJson();
	j["py"] = positionY.toJson();
	j["sx"] = scaleX.toJson();
	j["sy"] = scaleY.toJson();
	j["ro"] = rotation.toJson();
}

void AnimateableObjectBehaviour::drawEditor()
{
	if (beginKeyframeEditor())
	{
		drawSequences();

		endKeyframeEditor();
	}

	// Draw keyframe editor
	if (selectedKeyframe.has_value())
	{
		int index = selectedKeyframe.value().index;
		Sequence& sequence = *selectedKeyframe.value().sequence;

		Keyframe* kf;
		if (isKeyframeTimeEditing())
		{
			assert(timeEditingKeyframe.has_value());

			KeyframeTimeEditInfo& editInfo = timeEditingKeyframe.value();
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

		ImGui::DragFloat("Keyframe value", &kf->value, 0.1f);

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

void AnimateableObjectBehaviour::drawSequences()
{
	sequenceEdit(positionX, "Position X");
	sequenceEdit(positionY, "Position Y");
	sequenceEdit(scaleX, "Scale X");
	sequenceEdit(scaleY, "Scale Y");
	sequenceEdit(rotation, "Rotation");
}

bool AnimateableObjectBehaviour::beginKeyframeEditor()
{
	sequencesToDraw.clear();

	return true;
}

void AnimateableObjectBehaviour::sequenceEdit(Sequence& sequence, std::string label)
{
	sequencesToDraw.push_back(std::make_tuple(std::reference_wrapper(sequence), label));
}

void AnimateableObjectBehaviour::endKeyframeEditor()
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
	Level& level = *baseObject->level;
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

	float objectLength = baseObject->endTime - baseObject->startTime;

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
		newTime = std::clamp(newTime, 0.0f, objectLength);

		level.seek(newTime + baseObject->startTime);
	}

	// Zoom and pan
	if (ImGui::IsWindowFocused() && keyframeEditorRect.Contains(io.MousePos))
	{
		float visibleLength = endTime - startTime;
		float currentPos = (startTime + endTime) * 0.5f;

		float zoom = visibleLength / objectLength;

		zoom -= io.MouseWheel * 0.05f;

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
		{
			currentPos -= io.MouseDelta.x / size.x * visibleLength;
		}

		float minZoom = 2.0f / objectLength;
		zoom = std::clamp(zoom, minZoom, 1.0f);

		float newVisibleLength = zoom * objectLength;

		float minPos = newVisibleLength * 0.5f;
		float maxPos = objectLength - minPos;

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
			std::optional<KeyframeInfo> hoveredKeyframe;
			std::optional<KeyframeInfo> lastClickedKeyframe;

			// Input pass
			for (int i = 0; i < sequenceCount; i++)
			{
				ImVec2 baseSequenceCoord = keyframeEditorBase + ImVec2(0.0f, i * SEQUENCE_HEIGHT);
				Sequence& sequence = std::get<0>(sequencesToDraw[i]);

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
						KeyframeInfo kfInfo;
						kfInfo.index = j;
						kfInfo.sequence = &sequence;

						hoveredKeyframe = kfInfo;
					}

					if (kfRect.Contains(io.MouseClickedPos[0]))
					{
						KeyframeInfo kfInfo;
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
				Sequence& sequence = std::get<0>(sequencesToDraw[i]);
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
				Sequence& sequence = std::get<0>(sequencesToDraw[(io.MousePos.y - keyframeEditorBase.y) / SEQUENCE_HEIGHT]);
				float time = startTime + ((io.MousePos.x - keyframeEditorBase.x) / size.x) * (endTime - startTime);
				sequence.insertKeyframe(Keyframe(time, 0.0f, EaseType_Linear));
			}

			// Keyframe delete
			if (ImGui::IsWindowFocused() && selectedKeyframe.has_value() && ImGui::IsKeyDown(GLFW_KEY_DELETE))
			{
				KeyframeInfo& kfInfo = selectedKeyframe.value();
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
				KeyframeInfo& kfInfo = selectedKeyframe.value();

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

				KeyframeTimeEditInfo& editInfo = timeEditingKeyframe.value();
				Keyframe& kf = editInfo.keyframes[editInfo.index];

				float timeDelta = (io.MouseDelta.x / size.x) * (endTime - startTime);
				timeDelta = std::clamp(timeDelta, -kf.time, baseObject->endTime - kf.time);

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
		float pointerPos = (level.time - baseObject->startTime - startTime) / (endTime - startTime) * size.x;

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

void AnimateableObjectBehaviour::beginKeyframeTimeEdit(int index, Sequence& sequence)
{
	KeyframeTimeEditInfo editInfo;
	editInfo.index = index;
	editInfo.sequence = &sequence;
	editInfo.keyframes = sequence.keyframes;

	timeEditingKeyframe = editInfo;
}

void AnimateableObjectBehaviour::endKeyframeTimeEdit()
{
	assert(timeEditingKeyframe.has_value());

	KeyframeTimeEditInfo& info = timeEditingKeyframe.value();
	Keyframe kf = info.keyframes[info.index];

	auto it = std::find(info.sequence->keyframes.begin(), info.sequence->keyframes.end(), kf);

	KeyframeInfo newInfo;
	newInfo.index = it - info.sequence->keyframes.begin();
	newInfo.sequence = info.sequence;

	selectedKeyframe = newInfo;

	timeEditingKeyframe.reset();
}

void AnimateableObjectBehaviour::updateKeyframeTimeEdit()
{
	assert(timeEditingKeyframe.has_value());

	KeyframeTimeEditInfo& editInfo = timeEditingKeyframe.value();
	editInfo.sequence->loadKeyframes(editInfo.keyframes);
}

bool AnimateableObjectBehaviour::isKeyframeTimeEditing()
{
	return timeEditingKeyframe.has_value();
}
