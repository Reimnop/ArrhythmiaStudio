#include "TextObjectBehaviour.h"

#include "utils.h"
#include "../factories/ShapeFactory.h"
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
	renderer->color.x = 1.0f;
	renderer->color.y = 1.0f;
	renderer->color.z = 1.0f;

	baseObject->node->renderer = renderer;
}

LevelObjectBehaviour* TextObjectBehaviour::create(LevelObject* object)
{
	return new TextObjectBehaviour(object);
}

void TextObjectBehaviour::update(float time)
{
	AnimateableObjectBehaviour::update(time);

	renderer->color.w = opacity.update(time - baseObject->startTime);
}

void TextObjectBehaviour::readJson(json& j)
{
	text = j["text"].get<std::string>();
	renderer->setText(text);

	AnimateableObjectBehaviour::readJson(j);
	opacity.fromJson(j["op"]);
}

void TextObjectBehaviour::writeJson(json& j)
{
	j["text"] = text;

	AnimateableObjectBehaviour::writeJson(j);
	j["op"] = opacity.toJson();
}

void TextObjectBehaviour::drawEditor()
{
	ImGui::InputTextMultiline("Text", &text);

	if (ImGui::IsItemEdited())
	{
		renderer->setText(text);
	}

	AnimateableObjectBehaviour::drawEditor();
}

void TextObjectBehaviour::drawSequences()
{
	AnimateableObjectBehaviour::drawSequences();

	sequenceEdit(opacity, "Opacity");
}
