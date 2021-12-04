#include "CameraShakeEvent.h"

#include "imgui/imgui.h"

CameraShakeEvent::CameraShakeEvent(Level* level) : LevelEvent(level)
{
	
}

CameraShakeEvent::~CameraShakeEvent()
{
	
}

std::string CameraShakeEvent::getTitle()
{
	return "Camera shake";
}

void CameraShakeEvent::update(float time)
{
	
}

void CameraShakeEvent::drawEditor()
{
	ImGui::Text("Drawing cam shake!");
}
