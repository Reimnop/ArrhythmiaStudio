#include "TextObjectBehaviour.h"

#include "utils.h"
#include "../factories/ShapeFactory.h"
#include "../GameManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

TextObjectBehaviour::TextObjectBehaviour(LevelObject* baseObject) : AnimateableColoredObjectBehaviour(baseObject), color(Level::inst)
{
	if (!font)
	{
		font = new Font("Assets/Inconsolata-Regular.ttf");
	}

	Keyframe kf1(0.0f, 1.0f, EaseType_Linear);
	ColorIndexKeyframe cikf0(0.0f, 0, EaseType_Linear);
	opacity = Sequence(1, &kf1);
	color = ColorIndexSequence(1, &cikf0, Level::inst);

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

	Color _color = color.update(time - baseObject->startTime);
	renderer->color.r = _color.r;
	renderer->color.g = _color.g;
	renderer->color.b = _color.b;
	renderer->color.w = opacity.update(time - baseObject->startTime);
}

void TextObjectBehaviour::readJson(json& j)
{
	text = j["text"].get<std::string>();
	renderer->setText(std::wstring(text.begin(), text.end()));

	AnimateableObjectBehaviour::readJson(j);
	opacity.fromJson(j["op"]);
	color.fromJson(j["co"]);
}

void TextObjectBehaviour::writeJson(json& j)
{
	j["text"] = text;

	AnimateableObjectBehaviour::writeJson(j);
	j["op"] = opacity.toJson();
	j["co"] = color.toJson();
}

void TextObjectBehaviour::drawEditor()
{
	ImGui::InputTextMultiline("Text", &text);

	if (ImGui::IsItemEdited()) 
	{
		renderer->setText(std::wstring(text.begin(), text.end()));
	}

	AnimateableColoredObjectBehaviour::drawEditor();
}

void TextObjectBehaviour::drawSequences()
{
	AnimateableObjectBehaviour::drawSequences();

	sequenceEdit(opacity, "Opacity");
}

void TextObjectBehaviour::drawColorSequences()
{
	colorSequenceEdit(color, "Color");
}
