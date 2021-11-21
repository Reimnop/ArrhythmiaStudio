#include "AnimateableObjectBehaviour.h"

#include "imgui/imgui.h"

AnimateableObjectBehaviour::AnimateableObjectBehaviour(LevelObject* baseObject) : LevelObjectBehaviour(baseObject)
{
	
}

AnimateableObjectBehaviour::~AnimateableObjectBehaviour()
{

}

void AnimateableObjectBehaviour::update(float time)
{
	
}

void AnimateableObjectBehaviour::fromJson(json& j)
{
	
}

void AnimateableObjectBehaviour::toJson(json& j)
{
	
}

void AnimateableObjectBehaviour::drawEditor()
{
	ImGui::Text("The Editor is being drawn on Animateable");
}
