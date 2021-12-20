#include "TextObjectBehaviour.h"

#include "utils.h"
#include "../factories/ShapeFactory.h"
#include "../GameManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

TextObjectBehaviour::TextObjectBehaviour(LevelObject* baseObject) : AnimateableObjectBehaviour(baseObject)
{
	if (!font)
	{
		font = new Font("Assets/Inconsolata.asfont");
	}

	Keyframe kf1 = Keyframe(0.0f, 1.0f, EaseType_Linear);
	opacity = Sequence(1, &kf1);

	renderer = new TextRenderer(font);

	baseObject->node->renderer = renderer;
}

LevelObjectBehaviour* TextObjectBehaviour::create(LevelObject* object)
{
	return new TextObjectBehaviour(object);
}

void TextObjectBehaviour::update(float time)
{
	AnimateableObjectBehaviour::update(time);

	Level& level = *GameManager::inst->level;

	renderer->color.r = level.colorSlots[colorSlot]->color.r;
	renderer->color.g = level.colorSlots[colorSlot]->color.g;
	renderer->color.b = level.colorSlots[colorSlot]->color.b;
	renderer->color.w = opacity.update(time - baseObject->startTime);
}

void TextObjectBehaviour::readJson(json& j)
{
	colorSlot = j["color"].get<int>();
	text = j["text"].get<std::string>();
	renderer->setText(std::wstring(text.begin(), text.end()));

	AnimateableObjectBehaviour::readJson(j);
	opacity.fromJson(j["op"]);
}

void TextObjectBehaviour::writeJson(json& j)
{
	j["color"] = colorSlot;
	j["text"] = text;

	AnimateableObjectBehaviour::writeJson(j);
	j["op"] = opacity.toJson();
}

void TextObjectBehaviour::drawEditor()
{
	Level& level = *GameManager::inst->level;
	ImGui::SliderInt("Color", &colorSlot, 0, level.colorSlots.size());

	ImGui::InputTextMultiline("Text", &text);

	if (ImGui::IsItemEdited()) 
	{
		renderer->setText(std::wstring(text.begin(), text.end()));
	}

	AnimateableObjectBehaviour::drawEditor();
}

void TextObjectBehaviour::drawSequences()
{
	AnimateableObjectBehaviour::drawSequences();

	sequenceEdit(opacity, "Opacity");
}
