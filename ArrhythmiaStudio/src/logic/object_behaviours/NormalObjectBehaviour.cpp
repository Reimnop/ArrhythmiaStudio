#include "NormalObjectBehaviour.h"

#include "utils.h"
#include "../factories/ShapeFactory.h"
#include "../GameManager.h"
#include "imgui/imgui.h"

NormalObjectBehaviour::NormalObjectBehaviour(LevelObject* baseObject) : AnimateableColoredObjectBehaviour(baseObject), color(baseObject->level)
{
	Keyframe kf1(0.0f, 1.0f, EaseType_Linear);
	ColorIndexKeyframe cikf0(0.0f, 0, EaseType_Linear);
	opacity = Sequence(1, &kf1);
	color = ColorIndexSequence(1, &cikf0, baseObject->level);

	renderer = new MeshRenderer();

	baseObject->node->renderer = renderer;

	setShape("square");
}

LevelObjectBehaviour* NormalObjectBehaviour::create(LevelObject* object)
{
	return new NormalObjectBehaviour(object);
}

void NormalObjectBehaviour::update(float time) 
{
	AnimateableColoredObjectBehaviour::update(time);

	Color _color = color.update(time - baseObject->startTime);
	renderer->color.r = _color.r;
	renderer->color.g = _color.g;
	renderer->color.b = _color.b;
	renderer->color.w = opacity.update(time - baseObject->startTime);
}

void NormalObjectBehaviour::readJson(json& j)
{
	setShape(j["shape"].get<std::string>());
	AnimateableColoredObjectBehaviour::readJson(j);
	opacity.fromJson(j["op"]);
	color.fromJson(j["co"]);
}

void NormalObjectBehaviour::writeJson(json& j)
{
	j["shape"] = shape.id;
	AnimateableColoredObjectBehaviour::writeJson(j);
	j["op"] = opacity.toJson();
	j["co"] = color.toJson();
}

void NormalObjectBehaviour::drawEditor()
{
	if (ImGui::BeginCombo("Shape", shape.name.c_str()))
	{
		for (std::string id : ShapeFactory::getShapeIds())
		{
			Shape currentShape = ShapeFactory::getShape(id);

			if (ImGui::Selectable(currentShape.name.c_str(), currentShape.id == shape.id))
			{
				setShape(currentShape.id);
			}
		}
		ImGui::EndCombo();
	}

	AnimateableColoredObjectBehaviour::drawEditor();
}

void NormalObjectBehaviour::drawSequences()
{
	AnimateableColoredObjectBehaviour::drawSequences();

	sequenceEdit(opacity, "Opacity");
}

void NormalObjectBehaviour::drawColorSequences()
{
	colorSequenceEdit(color, "Color");
}

void NormalObjectBehaviour::setShape(std::string id)
{
	shape = ShapeFactory::getShape(id);
	renderer->mesh = shape.mesh;
}
