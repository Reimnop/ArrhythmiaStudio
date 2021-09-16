#include "Timeline.h"
#include "LevelManager.h"

#include <functional>
#include <algorithm>
#include <sstream>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <Windows.h>

#include "../rendering/ImGuiController.h"
#include "GlobalConstants.h"
#include "UndoRedoManager.h"
#include "utils.h"
#include "imgui/imgui_editorlib.h"
#include "undo_commands/AddObjectCmd.h"
#include "undo_commands/RemoveObjectCmd.h"
#include "undo_commands/MultiUndoCmd.h"

Timeline* Timeline::inst;

Timeline::Timeline()
{
	if (inst)
	{
		return;
	}

	inst = this;

	startTime = 0.0f;
	endTime = 10.0f;

	waveformTex = new Texture2D(512, 512, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	waveformShader = new ComputeShader("Assets/Shaders/waveform.comp");

	glGenBuffers(1, &audioBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, audioBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	ImGuiController::onLayout.push_back(std::bind(&Timeline::onLayout, this));
}

void Timeline::genBuffer(AudioClip* clip)
{
	int pixCount = clip->getLength() * EDITOR_WAVEFORM_FREQ;

	float* samples = new float[pixCount];
	for (int i = 0; i < pixCount; i++)
	{
		float t1 = i / (float)pixCount;
		float t2 = (i + 1) / (float)pixCount;

		int i1 = (int)std::lerp(0.0f, clip->samplesCount - 1, t1);
		int i2 = (int)std::lerp(0.0f, clip->samplesCount - 1, t2);

		float s = 0.0f;
		for (int j = i1; j < i2; j++)
		{
			s += std::abs(clip->samples[j] / 32768.0f);
		}
		samples[i] = s / (i2 - i1);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, audioBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, pixCount * sizeof(float), samples, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] samples;
}

void Timeline::onLayout()
{
	LevelManager* levelManager = LevelManager::inst;

	// Open a sequence window
	if (ImGui::Begin("Timeline"))
	{
		std::string time = timeToString(levelManager->time);
		ImGui::Text(time.c_str());

		ImGui::SameLine();
		if (playButton(levelManager->audioClip->isPlaying()))
		{
			AudioClip* clip = levelManager->audioClip;

			if (clip->isPlaying())
			{
				clip->pause();
			}
			else
			{
				clip->play();
			}
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);

		float speed = levelManager->audioClip->getSpeed();
		ImGui::SliderFloat("Speed", &speed, 0.25f, 4.0f);

		if (ImGui::IsItemEdited())
		{
			levelManager->audioClip->setSpeed(speed);
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::SliderInt("Layer", &layer, 0, EDITOR_MAX_OBJECT_LAYER);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::DragFloat("Instantiation Offset", &instantiationOffset, 0.1f);

		glm::ivec2 timelineSize;

		// Draw the timeline (huge code!)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImGuiStyle& style = ImGui::GetStyle();
			ImGuiIO& io = ImGui::GetIO();

			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			float availX = ImGui::GetContentRegionAvailWidth();

			ImVec2 timelineMin = ImVec2(cursorPos.x, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT);
			float timelineHeight = EDITOR_TIMELINE_BIN_COUNT * EDITOR_BIN_HEIGHT;

			ImVec2 clipSize = ImVec2(availX, timelineHeight);
			ImVec2 timelineMax = ImVec2(timelineMin.x + clipSize.x, timelineMin.y + clipSize.y);

			timelineSize = glm::ivec2((int)clipSize.x, (int)clipSize.y);

			// Draw editor bins
			drawList->PushClipRect(timelineMin, timelineMax, true);

			for (int i = 0; i < EDITOR_TIMELINE_BIN_COUNT; i++)
			{
				ImVec2 binMin = ImVec2(timelineMin.x, timelineMin.y + i * EDITOR_BIN_HEIGHT);
				ImVec2 binSize = ImVec2(availX, EDITOR_BIN_HEIGHT);

				drawList->AddRectFilled(
					binMin,
					ImVec2(binMin.x + binSize.x, binMin.y + binSize.y),
					i % 2 ? EDITOR_BIN_SECONDARY_COL : EDITOR_BIN_PRIMARY_COL);
			}

			// Draw waveform
			if (levelManager->audioClip)
			{
				drawList->AddImage((uint32_t*)waveformTex->getHandle(), ImVec2(timelineMin.x, timelineMax.y),
					ImVec2(timelineMax.x, timelineMin.y));
			}

			bool isTimelineHovered = ImGui::IntersectAABB(timelineMin, timelineMax, io.MousePos);

			// Timeline move and zoom
			if (ImGui::IsWindowFocused() && isTimelineHovered)
			{
				float songLength = levelManager->audioClip->getLength();

				float visibleLength = endTime - startTime;
				float currentPos = (startTime + endTime) * 0.5f;

				float zoom = visibleLength / songLength;

				zoom -= io.MouseWheel * 0.05f;

				if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
				{
					currentPos -= io.MouseDelta.x / availX * visibleLength;
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

			// Deselect
			if (isTimelineHovered && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
			{
				levelManager->selectedObjects.clear();
				Properties::inst->reset();
			}

			if (ImGui::IsWindowFocused() && ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_A))
			{
				for (const std::pair<uint64_t, LevelObject*> x : levelManager->level->levelObjects)
				{
					levelManager->selectedObjects.emplace(x.second);
				}
			}

			bool stripDragging = false;

			Level* level = levelManager->level;

			// Editor strips input pass
			for (auto it = level->levelObjects.rbegin(); it != level->levelObjects.rend(); it++)
			{
				LevelObject* levelObject = it->second;

				if (levelObject->layer != layer)
				{
					continue;
				}

				if (levelObject->killTime < startTime || levelObject->startTime > endTime)
				{
					continue;
				}

				// Calculate start and end position in pixel
				float startPos = (levelObject->startTime - startTime) / (endTime - startTime) * availX;
				float endPos = (levelObject->killTime - startTime) / (endTime - startTime) * availX;

				// Calculate strip params
				ImVec2 stripMin = ImVec2(timelineMin.x + startPos, timelineMin.y + levelObject->editorBinIndex * EDITOR_BIN_HEIGHT);
				ImVec2 stripMax = ImVec2(timelineMin.x + endPos, timelineMin.y + (levelObject->editorBinIndex + 1) * EDITOR_BIN_HEIGHT);

				const char* name = levelObject->name.c_str();

				ImGui::PushID(it->first);

				bool pressed;
				bool highlighted;
				if (ImGui::EditorStripInputPass(name, stripMin, stripMax, levelManager->selectedObjects.count(levelObject), &pressed, &highlighted))
				{
					levelManager->selectedObjects.emplace(levelObject);

					if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					{
						stripDragging = true;
					}
				}

				levelObject->timelineHighlighted = highlighted;

				ImGui::PopID();
			}

			// Dragging strips
			if (stripDragging) 
			{
				float minSt = INFINITY;
				for (LevelObject* obj : levelManager->selectedObjects)
				{
					minSt = std::min(minSt, obj->startTime);
				}

				ImVec2 delta = io.MouseDelta;
				float timeDelta = (delta.x / availX) * (endTime - startTime);
				timeDelta = std::max(timeDelta, -minSt);

				for (LevelObject* obj : levelManager->selectedObjects)
				{
					obj->startTime += timeDelta;
					obj->killTime += timeDelta;

					levelManager->recalculateObjectAction(obj);
				}

				levelManager->recalculateActionIndex(levelManager->time);
			}

			// Editor strips visual pass
			for (auto it = level->levelObjects.begin(); it != level->levelObjects.end(); it++)
			{
				LevelObject* levelObject = it->second;

				if (levelObject->layer != layer)
				{
					continue;
				}

				if (levelObject->killTime < startTime || levelObject->startTime > endTime)
				{
					continue;
				}

				// Calculate start and end position in pixel
				float startPos = (levelObject->startTime - startTime) / (endTime - startTime) * availX;
				float endPos = (levelObject->killTime - startTime) / (endTime - startTime) * availX;

				// Calculate strip params
				ImVec2 stripMin = ImVec2(timelineMin.x + startPos, timelineMin.y + levelObject->editorBinIndex * EDITOR_BIN_HEIGHT);
				ImVec2 stripMax = ImVec2(timelineMin.x + endPos, timelineMin.y + (levelObject->editorBinIndex + 1) * EDITOR_BIN_HEIGHT);

				const char* name = levelObject->name.c_str();

				ImGui::EditorStripVisualPass(name, stripMin, stripMax, levelObject->timelineHighlighted);
			}

			// Timeline borders
			ImU32 borderCol = ImGui::GetColorU32(ImGuiCol_Border);
			drawList->AddRect(timelineMin, ImVec2(timelineMin.x + availX, timelineMin.y + timelineHeight), borderCol);

			drawList->PopClipRect();

			// Time pointer input handling
			// Jumping the time pointer
			ImGui::SetCursorScreenPos(cursorPos);
			if (ImGui::InvisibleButton("##TimePointer", ImVec2(availX, EDITOR_TIME_POINTER_HEIGHT)))
			{
				float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
				newTime = std::clamp(newTime, 0.0f, levelManager->audioClip->getLength());

				levelManager->audioClip->pause();

				levelManager->updateLevel(newTime);
				levelManager->audioClip->seek(newTime);
			}

			// Dragging time pointer
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				float newTime = startTime + (io.MousePos.x - cursorPos.x) / availX * (endTime - startTime);
				newTime = std::clamp(newTime, 0.0f, levelManager->audioClip->getLength());

				levelManager->audioClip->pause();

				levelManager->updateLevel(newTime);
				levelManager->audioClip->seek(newTime);
			}

			// Drawing the time pointer
			drawList->PushClipRect(cursorPos, ImVec2(cursorPos.x + availX,
			                                         cursorPos.y + timelineHeight + EDITOR_TIME_POINTER_HEIGHT), true);

			// Draw frame of the time pointer
			drawList->AddRect(cursorPos, ImVec2(cursorPos.x + availX, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT),
			                  borderCol);

			// Draw time pointer
			constexpr float pointerRectHeight = EDITOR_TIME_POINTER_HEIGHT - EDITOR_TIME_POINTER_TRI_HEIGHT;

			float pointerPos = cursorPos.x + (levelManager->time - startTime) / (endTime - startTime) * availX;
			drawList->AddLine(ImVec2(pointerPos, cursorPos.y),
			                  ImVec2(pointerPos, cursorPos.y + timelineHeight + EDITOR_TIME_POINTER_HEIGHT), borderCol);

			drawList->AddRectFilled(
				ImVec2(pointerPos - EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y),
				ImVec2(pointerPos + EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y + pointerRectHeight),
				borderCol);

			drawList->AddTriangleFilled(
				ImVec2(pointerPos - EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y + pointerRectHeight),
				ImVec2(pointerPos, cursorPos.y + EDITOR_TIME_POINTER_HEIGHT),
				ImVec2(pointerPos + EDITOR_TIME_POINTER_WIDTH * 0.5f, cursorPos.y + pointerRectHeight),
				borderCol);

			drawList->PopClipRect();

			// Object delete
			if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_DELETE))
			{
				if (!levelManager->selectedObjects.empty())
				{
					std::vector<RemoveObjectCmd*> cmds;
					for (LevelObject* obj : levelManager->selectedObjects) 
					{
						cmds.push_back(new RemoveObjectCmd(obj));
					}
					UndoRedoManager::inst->push(new MultiUndoCmd(cmds));

					for (LevelObject* obj : levelManager->selectedObjects)
					{
						levelManager->removeObject(obj);
					}

					levelManager->recalculateActionIndex(levelManager->time);

					levelManager->selectedObjects.clear();
				}
			}

			// Object copy
			if (ImGui::IsWindowFocused() && ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_C) && !levelManager->selectedObjects.empty())
			{
				OpenClipboard(NULL);
				EmptyClipboard();

				float minSt = INFINITY;
				for (LevelObject* obj : levelManager->selectedObjects)
				{
					minSt = std::min(minSt, obj->startTime);
				}

				nlohmann::json j = nlohmann::json::array();
				int i = 0;
				for (LevelObject* obj : levelManager->selectedObjects)
				{
					nlohmann::json jObj = obj->toJson();
					jObj["start"] = jObj["start"].get<float>() - minSt;
					jObj["kill"] = jObj["kill"].get<float>() - minSt;

					j[i] = jObj;
					i++;
				}

				std::stringstream ss;
				ss << j;

				std::string jStr = ss.str();

				HGLOBAL pGlobal = GlobalAlloc(GMEM_MOVEABLE, jStr.size() + 1);
				HGLOBAL lGlobal = GlobalLock(pGlobal);
				memcpy(lGlobal, jStr.c_str(), jStr.size() + 1);
				GlobalUnlock(pGlobal);

				SetClipboardData(EDITOR_FORMAT_OBJECT, pGlobal);

				CloseClipboard();

				GlobalFree(pGlobal);
			}

			// Object paste
			if (ImGui::IsWindowFocused() && ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(GLFW_KEY_V))
			{
				if (IsClipboardFormatAvailable(EDITOR_FORMAT_OBJECT))
				{
					OpenClipboard(NULL);

					HGLOBAL pGlobal = GetClipboardData(EDITOR_FORMAT_OBJECT);

					if (pGlobal)
					{
						const char* jsonPtr = (const char*)GlobalLock(pGlobal);
						if (jsonPtr)
						{
							std::string jsonStr(jsonPtr);

							nlohmann::json objsJson = nlohmann::json::parse(jsonPtr);

							// Generate new ids
							std::unordered_map<uint64_t, uint64_t> idMap;
							for (int i = 0; i < objsJson.size(); i++)
							{
								idMap[objsJson[i]["id"].get<uint64_t>()] = Utils::randomId();
							}

							for (int i = 0; i < objsJson.size(); i++)
							{
								uint64_t oldId = objsJson[i]["id"].get<uint64_t>();
								objsJson[i]["id"] = idMap[oldId];

								uint64_t oldPid = objsJson[i]["parent"].get<uint64_t>();
								if (idMap.count(oldPid))
								{
									objsJson[i]["parent"] = idMap[oldPid];
								}
							}

							std::vector<LevelObject*> newObjects;

							for (nlohmann::json objJson : objsJson)
							{
								objJson["start"] = objJson["start"].get<float>() + levelManager->time + instantiationOffset;
								objJson["kill"] = objJson["kill"].get<float>() + levelManager->time + instantiationOffset;

								LevelObject* newObject = new LevelObject(objJson);

								levelManager->insertObject(newObject);
								newObjects.push_back(newObject);
							}

							for (LevelObject* obj : newObjects)
							{
								if (!levelManager->initializeObjectParent(obj))
								{
									obj->parentId = 0;
								}
							}

							levelManager->recalculateActionIndex(levelManager->time);

							GlobalUnlock(pGlobal);
						}
					}

					CloseClipboard();
				}
			}

			// Play/Pause
			if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(GLFW_KEY_SPACE))
			{
				AudioClip* clip = levelManager->audioClip;

				if (clip->isPlaying())
				{
					clip->pause();
				}
				else
				{
					clip->play();
				}
			}

			// New object popup
			ImGui::SetCursorScreenPos(timelineMin);
			ImGui::InvisibleButton("##NewObjectBtn", clipSize);

			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("new-popup");
			}

			if (ImGui::BeginPopup("new-popup"))
			{
				if (ImGui::Selectable("New Object"))
				{
					LevelObject* newObject = new LevelObject();
					newObject->startTime = levelManager->time + instantiationOffset;
					newObject->killTime = levelManager->time + 5.0f + instantiationOffset;
					newObject->layer = layer;

					levelManager->insertObject(newObject);
					levelManager->recalculateActionIndex(levelManager->time);

					UndoRedoManager::inst->push(new AddObjectCmd(newObject));
				}

				ImGui::EndPopup();
			}

			// Reset cursor
			ImGui::SetCursorScreenPos(cursorPos);
			ImGui::ItemSize(ImVec2(availX, timelineHeight + EDITOR_TIME_POINTER_HEIGHT));
		}

		if (timelineSize != oldWaveformSize)
		{
			waveformTex->resize(timelineSize.x, timelineSize.y);
			oldWaveformSize = timelineSize;
		}

		// Compute waveform
		if (levelManager->audioClip)
		{
			glUseProgram(waveformShader->getHandle());

			glBindImageTexture(0, waveformTex->getHandle(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA8);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, audioBuffer);

			glUniform1f(0, startTime);
			glUniform1f(1, endTime);
			glUniform1f(2, EDITOR_WAVEFORM_FREQ);

			glDispatchCompute(std::ceil(timelineSize.x / 8.0f), std::ceil(timelineSize.y / 8.0f), 1);
		}
	}
	ImGui::End();
}

bool Timeline::playButton(bool playing)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 cursor = ImGui::GetCursorScreenPos();

	if (playing)
	{
		drawList->AddRectFilled(
			cursor,
			ImVec2(cursor.x + EDITOR_PLAY_BUTTON_SIZE / 3.0f, cursor.y + EDITOR_PLAY_BUTTON_SIZE),
			EDITOR_PLAY_BUTTON_COL);
		drawList->AddRectFilled(
			ImVec2(cursor.x + EDITOR_PLAY_BUTTON_SIZE / 3.0f * 2.0f, cursor.y),
			ImVec2(cursor.x + EDITOR_PLAY_BUTTON_SIZE, cursor.y + EDITOR_PLAY_BUTTON_SIZE),
			EDITOR_PLAY_BUTTON_COL);
	}
	else
	{
		drawList->AddTriangleFilled(
			cursor,
			ImVec2(cursor.x, cursor.y + EDITOR_PLAY_BUTTON_SIZE),
			ImVec2(cursor.x + EDITOR_PLAY_BUTTON_SIZE, cursor.y + EDITOR_PLAY_BUTTON_SIZE * 0.5f),
			EDITOR_PLAY_BUTTON_COL);
	}

	return ImGui::InvisibleButton("##PlayButton", ImVec2(EDITOR_PLAY_BUTTON_SIZE, EDITOR_PLAY_BUTTON_SIZE));
}

std::string Timeline::timeToString(float time) const
{
	float secs = std::floor(time);
	float frac = time - secs;

	int milliseconds = (int)(frac * 1000.0f) % 1000;
	int seconds = (int)secs % 60;
	int minutes = (int)secs / 60 % 60;
	int hours = (int)secs / 3600 % 60;

	std::string millisecondsStr =
		milliseconds < 10
			? "00" + std::to_string(milliseconds)
			: milliseconds < 100
			? "0" + std::to_string(milliseconds)
			: std::to_string(milliseconds);
	std::string secondsStr = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
	std::string minutesStr = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
	std::string hoursStr = hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);

	return hoursStr + ":" + minutesStr + ":" + secondsStr + "." + millisecondsStr;
}