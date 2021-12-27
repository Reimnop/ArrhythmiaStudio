#include "Timeline.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "../factories/ObjectBehaviourFactory.h"
#include "../LevelObject.h"
#include "../GameManager.h"
#include "utils.h"

Timeline::Timeline()
{
	waveformTex = new Texture2D(512, 512, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	waveformShader = new ComputeShader("Assets/Shaders/waveform.comp");

	genBuffer();
}

Timeline::~Timeline()
{
	delete waveformShader;
	delete waveformTex;
}

void Timeline::genBuffer()
{
	AudioClip& clip = *GameManager::inst->level->clip;

	int pixCount = clip.getLength() * WAVEFORM_FREQ;

	float* samples = new float[pixCount];
	for (int i = 0; i < pixCount; i++)
	{
		float t1 = i / (float)pixCount;
		float t2 = (i + 1) / (float)pixCount;

		int i1 = (int)std::lerp(0.0f, clip.samplesCount - 1, t1);
		int i2 = (int)std::lerp(0.0f, clip.samplesCount - 1, t2);

		float s = 0.0f;
		for (int j = i1; j < i2; j++)
		{
			s += std::abs(clip.samples[j] / 32768.0f);
		}
		samples[i] = s / (i2 - i1);
	}

	glGenBuffers(1, &audioBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, audioBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, pixCount * sizeof(float), samples, GL_STATIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] samples;
}

std::string Timeline::getTitle()
{
	return "Timeline";
}

void Timeline::draw()
{
	GameManager& gameManager = *GameManager::inst;
	Level& level = *gameManager.level;

	ImGui::Text(Utils::timeToString(level.time).c_str());

	ImGui::SameLine();

	// Draw play button
	if (playButton(level.clip->isPlaying()))
	{
		if (level.clip->isPlaying())
		{
			level.clip->pause();
		}
		else
		{
			level.clip->play();
		}
	}

	ImGui::SameLine();

	// Volume
	ImGui::SetNextItemWidth(150.0f);
	float volume = level.clip->getVolume();
	ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);

	if (ImGui::IsItemEdited())
	{
		level.clip->setVolume(volume);
	}

	ImGui::SameLine();

	// Speed
	ImGui::SetNextItemWidth(150.0f);
	float speed = level.clip->getSpeed();
	ImGui::SliderFloat("Speed", &speed, 0.25f, 4.0f);

	if (ImGui::IsItemEdited())
	{
		level.clip->setSpeed(speed);
	}

	ImGui::SameLine();

	// BPM
	ImGui::SetNextItemWidth(150.0f);

	ImGui::DragFloat("BPM", &level.bpm);

	ImGui::SameLine();

	// Offset
	ImGui::SetNextItemWidth(150.0f);

	ImGui::DragFloat("Offset", &level.offset);

	drawTimeline();

	// Compute timeline waveform
	if (timelineSize != oldTimelineSize)
	{
		waveformTex->resize(timelineSize.x, timelineSize.y);
		oldTimelineSize = timelineSize;
	}

	glUseProgram(waveformShader->getHandle());

	glBindImageTexture(0, waveformTex->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA8);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, audioBuffer);

	glUniform1f(0, startTime);
	glUniform1f(1, endTime);
	glUniform1f(2, WAVEFORM_FREQ);

	glDispatchCompute(std::ceil(timelineSize.x / 8.0f), std::ceil(timelineSize.y / 8.0f), 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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

	// Data
	ImGuiWindow& window = *ImGui::GetCurrentWindow();
	ImGuiIO& io = ImGui::GetIO();
	ImGuiContext& context = *ImGui::GetCurrentContext();
	GameManager& gameManager = *GameManager::inst;
	Level& level = *gameManager.level;

	// Coords calculation
	ImVec2 baseCoord = ImGui::GetCursorScreenPos();
	ImVec2 size = ImVec2(ImGui::GetContentRegionAvailWidth(), BIN_COUNT * BIN_HEIGHT + TIME_POINTER_HEIGHT);
	ImVec2 timePointerAreaSize = ImVec2(size.x, TIME_POINTER_HEIGHT);
	ImVec2 objectEditorSize = ImVec2(size.x, BIN_COUNT * BIN_HEIGHT);

	timelineSize = objectEditorSize;

	// Input handling
	ImRect pointerRect = ImRect(
		baseCoord,
		baseCoord + timePointerAreaSize);

	ImRect objectEditorRect = ImRect(
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

	// New object creation
	if (ImGui::BeginPopup("##timeline-context"))
	{
		if (ImGui::BeginMenu("New object"))
		{
			for (std::string id : ObjectBehaviourFactory::getBehaviorIds())
			{
				ObjectBehaviourInfo info = ObjectBehaviourFactory::getFromId(id);
				if (ImGui::MenuItem(info.name.c_str()))
				{
					float st = level.time;
					float et = st + 5.0f;
					LevelObject* obj = new LevelObject(id, &level);
					obj->startTime = st;
					obj->endTime = et;
					level.addObject(obj);
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && objectEditorRect.Contains(io.MousePos))
	{
		ImGui::OpenPopup("##timeline-context");
	}

	// Zoom and pan
	if (ImGui::IsWindowFocused() && objectEditorRect.Contains(io.MousePos))
	{
		float songLength = level.levelLength;

		float visibleLength = endTime - startTime;
		float currentPos = (startTime + endTime) * 0.5f;

		float zoom = visibleLength / songLength;

		zoom -= io.MouseWheel * 0.05f;

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
		{
			currentPos -= io.MouseDelta.x / size.x * visibleLength;
		}

		float minZoom = 2.0f / songLength;
		zoom = std::clamp(zoom, minZoom, 1.0f);

		float newVisibleLength = zoom * songLength;

		float minPos = newVisibleLength * 0.5f;
		float maxPos = songLength - minPos;

		currentPos = std::clamp(currentPos, minPos, maxPos);

		startTime = currentPos - newVisibleLength * 0.5f;
		endTime = currentPos + newVisibleLength * 0.5f;
	}

	// Drawing
	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	drawList.PushClipRect(baseCoord, baseCoord + size, true);
	{
		ImVec2 objectEditorBase = baseCoord + ImVec2(0.0f, timePointerAreaSize.y);

		// Draw background
		drawList.AddRectFilled(objectEditorBase, objectEditorBase + objectEditorSize, BIN_PRIMARY_COL);

		// Draw foreground
		for (int i = 1; i < BIN_COUNT; i += 2)
		{
			ImVec2 binBase = objectEditorBase + ImVec2(0.0f, BIN_HEIGHT * i);
			drawList.AddRectFilled(binBase, binBase + ImVec2(size.x, BIN_HEIGHT), BIN_SECONDARY_COL);
		}

		// Draw waveform
		drawList.AddImage(
			(ImTextureID)waveformTex->getHandle(), 
			objectEditorBase, 
			objectEditorBase + objectEditorSize,
			ImVec2(0.0f, 1.0f),
			ImVec2(1.0f, 0.0f));

		float spb = 60.0f / level.bpm;
		int gridCount = (endTime - startTime) / spb;

		// Draw background time grid
		// Instead of this check I could try using a shader but nah, that's overengineering
		if (gridCount < 200) 
		{
			for (float t = ceil((startTime - level.offset) / spb) * spb; t < endTime - level.offset; t += spb)
			{
				float pos = (t + level.offset - startTime) / (endTime - startTime) * size.x;
				drawList.AddLine(
					baseCoord + ImVec2(pos, TIME_POINTER_HEIGHT),
					baseCoord + ImVec2(pos, size.y),
					ImGui::GetColorU32(ImGuiCol_Border, 0.3));
			}
		}

		// Object strips handling
		{
			std::optional<std::reference_wrapper<LevelObject>> objectHovering;
			std::optional<std::reference_wrapper<LevelObject>> lastClickedObject;

			// Input pass
			for (auto it = level.levelObjects.begin(); it != level.levelObjects.end(); ++it)
			{
				LevelObject& object = *it->second;

				if (object.endTime < startTime || object.startTime > endTime || object.bin < 0 || object.bin >= BIN_COUNT)
				{
					continue;
				}

				float startPos = (object.startTime - startTime) / (endTime - startTime) * size.x;
				float endPos = (object.endTime - startTime) / (endTime - startTime) * size.x;

				ImRect stripRect = ImRect(
					objectEditorBase + ImVec2(startPos, BIN_HEIGHT * object.bin),
					objectEditorBase + ImVec2(endPos, BIN_HEIGHT * (object.bin + 1)));

				if (stripRect.Contains(io.MousePos) && ImGui::IsWindowHovered())
				{
					objectHovering = object;
				}

				if (stripRect.Contains(io.MouseClickedPos[0]) && ImGui::IsWindowHovered())
				{
					lastClickedObject = object;
				}
			}

			// Visual pass
			for (auto it = level.levelObjects.begin(); it != level.levelObjects.end(); ++it)
			{
				LevelObject& object = *it->second;

				if (object.endTime < startTime || object.startTime > endTime || object.bin < 0 || object.bin >= BIN_COUNT)
				{
					continue;
				}

				float startPos = (object.startTime - startTime) / (endTime - startTime) * size.x;
				float endPos = (object.endTime - startTime) / (endTime - startTime) * size.x;

				std::string objectName = object.getName();

				drawObjectStrip(
					objectName,
					objectEditorBase + ImVec2(startPos, BIN_HEIGHT * object.bin),
					objectEditorBase + ImVec2(endPos, BIN_HEIGHT * (object.bin + 1)),
					(level.selection.selectedObject.has_value() ? &object == &level.selection.selectedObject.value().get() : false) || (objectHovering.has_value() ? &objectHovering.value().get() == &object : false));
			}

			if (ImGui::IsWindowFocused() && objectEditorRect.Contains(io.MousePos) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				level.selection.selectedObject = objectHovering;
			}

			// Object dragging action
			ImGuiID objectDragID = window.GetID("##object-drag");

			if (ImGui::IsWindowFocused() && context.MovingWindow != &window && lastClickedObject.has_value() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				ImGui::SetActiveID(objectDragID, &window);
				ImGui::SetFocusID(objectDragID, &window);
				ImGui::FocusWindow(&window);

				level.selection.selectedObject = lastClickedObject;
			}

			if (context.ActiveId == objectDragID && context.ActiveIdSource == ImGuiInputSource_Mouse && !io.MouseDown[0])
			{
				ImGui::ClearActiveID();
			}

			if (context.ActiveId == objectDragID)
			{
				assert(level.selection.selectedObject.has_value());

				LevelObject& object = level.selection.selectedObject.value();

				float timeDelta = (io.MouseDelta.x / size.x) * (endTime - startTime);
				timeDelta = std::clamp(timeDelta, -object.startTime, level.levelLength - object.endTime);

				object.startTime += timeDelta;
				object.endTime += timeDelta;

				level.removeActivateList(&object);
				level.removeDeactivateList(&object);
				level.insertActivateList(&object);
				level.insertDeactivateList(&object);
				level.recalculateObjectsState();
			}
			else // Other things to do when not dragging
			{
				// Object deletion
				if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE) && level.selection.selectedObject.has_value())
				{
					LevelObject& object = level.selection.selectedObject.value();
					level.selection.selectedObject.reset();
					level.deleteObject(&object);
				}
			}
		}

		// Draw foreground time grid
		if (gridCount < 200)
		{
			for (float t = ceil((startTime - level.offset) / spb) * spb; t < endTime - level.offset; t += spb)
			{
				float pos = (t + level.offset - startTime) / (endTime - startTime) * size.x;
				drawList.AddLine(
					baseCoord + ImVec2(pos, TIME_POINTER_HEIGHT),
					baseCoord + ImVec2(pos, size.y),
					ImGui::GetColorU32(ImGuiCol_Border, 0.3));
			}
		}

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

void Timeline::drawObjectStrip(std::string& name, ImVec2 min, ImVec2 max, bool highlighted)
{
	// Configurations
	constexpr ImU32 INACTIVE_COL = 0xFFBABABA;
	constexpr ImU32 ACTIVE_COL = 0xFF626262;
	constexpr float TEXT_MARGIN_LEFT = 4.0f;
	constexpr float TEXT_MARGIN_RIGHT = 16.0f;
	constexpr float TEXT_RECT_MARGIN_LEFT = 8.0f;

	const char* textPtr = name.c_str();

	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	ImVec2 textSize = ImGui::CalcTextSize(textPtr);

	drawList.PushClipRect(min, max, true);

	drawList.AddRectFilled(min, max, highlighted ? ACTIVE_COL : INACTIVE_COL);
	drawList.AddRectFilled(
		min + ImVec2(TEXT_RECT_MARGIN_LEFT, 0.0f),
		ImVec2(min.x + textSize.x + TEXT_MARGIN_RIGHT, max.y), 
		highlighted ? INACTIVE_COL : ACTIVE_COL);
	drawList.AddText(
		min + ImVec2(TEXT_RECT_MARGIN_LEFT + TEXT_MARGIN_LEFT, 0.0f),
		highlighted ? ACTIVE_COL : INACTIVE_COL, 
		textPtr);

	drawList.PopClipRect();
}

bool Timeline::playButton(bool playing)
{
	constexpr float PLAY_BUTTON_SIZE = 20.0f;
	constexpr ImU32 PLAY_BUTTON_COL = 0xFFBABABA;

	ImDrawList& drawList = *ImGui::GetWindowDrawList();
	ImVec2 cursor = ImGui::GetCursorScreenPos();

	if (playing)
	{
		drawList.AddRectFilled(
			cursor,
			ImVec2(cursor.x + PLAY_BUTTON_SIZE / 3.0f, cursor.y + PLAY_BUTTON_SIZE),
			PLAY_BUTTON_COL);
		drawList.AddRectFilled(
			ImVec2(cursor.x + PLAY_BUTTON_SIZE / 3.0f * 2.0f, cursor.y),
			ImVec2(cursor.x + PLAY_BUTTON_SIZE, cursor.y + PLAY_BUTTON_SIZE),
			PLAY_BUTTON_COL);
	}
	else
	{
		drawList.AddTriangleFilled(
			cursor,
			ImVec2(cursor.x, cursor.y + PLAY_BUTTON_SIZE),
			ImVec2(cursor.x + PLAY_BUTTON_SIZE, cursor.y + PLAY_BUTTON_SIZE * 0.5f),
			PLAY_BUTTON_COL);
	}

	return ImGui::InvisibleButton("##play-button", ImVec2(PLAY_BUTTON_SIZE, PLAY_BUTTON_SIZE));
}
