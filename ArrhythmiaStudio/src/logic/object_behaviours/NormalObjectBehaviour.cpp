#include "NormalObjectBehaviour.h"

#include "utils.h"
#include "../factories/ShapeFactory.h"
#include "../GameManager.h"
#include "imgui/imgui.h"

NormalObjectBehaviour::NormalObjectBehaviour(LevelObject* baseObject) : AnimateableObjectBehaviour(baseObject)
{
	Keyframe kf1 = Keyframe(0.0f, 1.0f, EaseType_Linear);
	opacity = Sequence(1, &kf1);

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
	AnimateableObjectBehaviour::update(time);

	Level& level = *GameManager::inst->level;

	renderer->color.r = level.colorSlots[colorSlot]->color.r;
	renderer->color.g = level.colorSlots[colorSlot]->color.g;
	renderer->color.b = level.colorSlots[colorSlot]->color.b;
	renderer->color.w = opacity.update(time - baseObject->startTime);
}

void NormalObjectBehaviour::readJson(json& j)
{
	colorSlot = j["color"].get<int>();
	setShape(j["shape"].get<std::string>());
	AnimateableObjectBehaviour::readJson(j);
	opacity.fromJson(j["op"]);
}

void NormalObjectBehaviour::writeJson(json& j)
{
	j["color"] = colorSlot;
	j["shape"] = shape.id;
	AnimateableObjectBehaviour::writeJson(j);
	j["op"] = opacity.toJson();
}

void NormalObjectBehaviour::drawEditor()
{
	Level& level = *GameManager::inst->level;
	ImGui::SliderInt("Color", &colorSlot, 0, level.colorSlots.size());

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

	AnimateableObjectBehaviour::drawEditor();
}

void NormalObjectBehaviour::drawSequences()
{
	AnimateableObjectBehaviour::drawSequences();

	sequenceEdit(opacity, "Opacity");
}

void NormalObjectBehaviour::setShape(std::string id)
{
	shape = ShapeFactory::getShape(id);
	renderer->mesh = shape.mesh;
}
