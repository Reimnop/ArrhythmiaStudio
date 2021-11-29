#include "AnimateableObjectBehaviour.h"
#include "../Level.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

std::vector<std::reference_wrapper<Sequence>> AnimateableObjectBehaviour::sequencesToDraw;
std::optional<KeyframeInfo> AnimateableObjectBehaviour::selectedKeyframe;
std::optional<KeyframeTimeEditInfo> AnimateableObjectBehaviour::timeEditingKeyframe;

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
	transform.rotation = angleAxis(rotation.update(t) / PI * 180.0f, glm::vec3(0.0f, 0.0f, -1.0f));
}

void AnimateableObjectBehaviour::readJson(json& j)
{
	
}

void AnimateableObjectBehaviour::writeJson(json& j)
{
	
}

void AnimateableObjectBehaviour::drawEditor()
{
	if (beginKeyframeEditor())
	{
		sequenceEdit(positionX);
		sequenceEdit(positionY);
		sequenceEdit(scaleX);
		sequenceEdit(scaleY);
		sequenceEdit(rotation);

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

bool AnimateableObjectBehaviour::beginKeyframeEditor()
{
	sequencesToDraw.clear();

	return true;
}

void AnimateableObjectBehaviour::sequenceEdit(Sequence& sequence)
{
	sequencesToDraw.push_back(sequence);
}

void AnimateableObjectBehaviour::endKeyframeEditor()
{
	// Configurations
	constexpr float SEQUENCE_HEIGHT = 20.0f;
	constexpr ImU32 SEQUENCE_PRIMARY_COL = 0xFF1F1F1F;
	constexpr ImU32 SEQUENCE_SECONDARY_COL = 0xFF2E2E2E;

	constexpr float TIME_POINTER_WIDTH = 18.0f;
	constexpr float TIME_POINTER_HEIGHT = 30.0f;
	constexpr float TIME_POINTER_TRI_HEIGHT = 8.0f;

	constexpr float KEYFRAME_SIZE = 20.0f;
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

	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && pointerRect.Contains(io.MousePos))
	{
		ImGui::SetActiveID(pointerID, &window);
		ImGui::SetFocusID(pointerID, &window);
		ImGui::FocusWindow(&window);
	}

	if (context.ActiveId == pointerID && context.ActiveIdSource == ImGuiInputSource_Mouse && !io.MouseDown[ImGuiMouseButton_Left])
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

			// Input pass
			for (int i = 0; i < sequenceCount; i++)
			{
				ImVec2 baseSequenceCoord = keyframeEditorBase + ImVec2(0.0f, i * SEQUENCE_HEIGHT);
				Sequence& sequence = sequencesToDraw[i];

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
				}
			}

			// Draw pass
			for (int i = 0; i < sequenceCount; i++)
			{
				ImVec2 baseSequenceCoord = keyframeEditorBase + ImVec2(0.0f, i * SEQUENCE_HEIGHT);
				Sequence& sequence = sequencesToDraw[i];

				for (int j = 0; j < sequence.keyframes.size(); j++)
				{
					float kfPos = (sequence.keyframes[j].time - startTime) / (endTime - startTime) * size.x;
					if (kfPos < -HALF_KEYFRAME_SIZE || kfPos > size.x + HALF_KEYFRAME_SIZE)
					{
						continue;
					}

					bool isHighlighted = 
						(hoveredKeyframe.has_value() ?
						hoveredKeyframe.value().index == j && hoveredKeyframe.value().sequence == &sequence : false) ||
						(selectedKeyframe.has_value() ?
						selectedKeyframe.value().index == j && selectedKeyframe.value().sequence == &sequence : false);

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

			// Keyframe dragging action
			ImGuiID keyframeDragID = window.GetID("##keyframe-drag");

			if (ImGui::IsWindowFocused() && hoveredKeyframe.has_value() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				ImGui::SetActiveID(keyframeDragID, &window);
				ImGui::SetFocusID(keyframeDragID, &window);
				ImGui::FocusWindow(&window);

				selectedKeyframe = hoveredKeyframe;
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
