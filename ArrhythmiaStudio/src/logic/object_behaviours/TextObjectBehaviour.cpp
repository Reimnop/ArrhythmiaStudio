#include "TextObjectBehaviour.h"

#include "utils.h"
#include "../factories/FontFactory.h"
#include "../GameManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

TextObjectBehaviour::TextObjectBehaviour(LevelObject* baseObject) : AnimateableColoredObjectBehaviour(baseObject), color(Level::inst)
{
	Keyframe kf1(0.0f, 1.0f, EaseType_Linear);
	ColorIndexKeyframe cikf0(0.0f, 0, EaseType_Linear);
	opacity = Sequence(1, &kf1);
	color = ColorIndexSequence(1, &cikf0, Level::inst);

	renderer = new TextRenderer();
	setFont("inconsolata");

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

void TextObjectBehaviour::setFont(std::string font)
{
	this->font = font;
	renderer->setFont(FontFactory::getFont(font));
}

void TextObjectBehaviour::readJson(json& j)
{
	text = j["text"].get<std::string>();
	setFont(j["font"].get<std::string>());
	renderer->setText(text);

	AnimateableObjectBehaviour::readJson(j);
	opacity.fromJson(j["op"]);
	color.fromJson(j["co"]);
}

void TextObjectBehaviour::writeJson(json& j)
{
	j["text"] = text;
	j["font"] = font;

	AnimateableObjectBehaviour::writeJson(j);
	j["op"] = opacity.toJson();
	j["co"] = color.toJson();
}

void TextObjectBehaviour::drawEditor()
{
	ImGui::InputTextMultiline("Text", &text);

	if (ImGui::IsItemEdited()) 
	{
		renderer->setText(text);
	}

	if (ImGui::BeginCombo("Font", FontFactory::getFont(font)->name.c_str()))
	{
		std::vector<std::string> ids = FontFactory::getFontIds();
		for (std::string id : ids)
		{
			if (ImGui::Selectable(FontFactory::getFont(id)->name.c_str(), font == id))
			{
				setFont(id);
			}
		}

		ImGui::EndCombo();
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
