#include "NormalObjectBehaviour.h"
#include "../factories/ShapeFactory.h"
#include "imgui/imgui.h"

Shader* shader;

NormalObjectBehaviour::NormalObjectBehaviour(LevelObject* baseObject) : AnimateableObjectBehaviour(baseObject)
{
	Keyframe kf1 = Keyframe(0.0f, 1.0f, EaseType_Linear);
	opacity = Sequence(1, &kf1);

	// TODO: REMOVE DEBUG CODE!
	MaterialProperty properties[]
	{
		MaterialProperty("Color", MaterialPropertyType_Vector3, 12)
	};

	Material* material = new Material(1, properties);
	material->setVec3("Color", glm::vec3(1.0f));
	material->updateBuffer();

	if (!shader) {
		shader = new Shader("Assets/Shaders/unlit.vert", "Assets/Shaders/unlit.frag");
	}

	renderer = new MeshRenderer();
	renderer->material = material;
	renderer->shader = shader;

	baseObject->node->renderer = renderer;

	setShape("square");
}

NormalObjectBehaviour::~NormalObjectBehaviour()
{
	delete baseObject->node->renderer;
}

LevelObjectBehaviour* NormalObjectBehaviour::create(LevelObject* object)
{
	return new NormalObjectBehaviour(object);
}

void NormalObjectBehaviour::update(float time) 
{
	AnimateableObjectBehaviour::update(time);

	renderer->opacity = opacity.update(time - baseObject->startTime);
}

void NormalObjectBehaviour::readJson(json& j)
{
	setShape(j["shape"].get<std::string>());
	AnimateableObjectBehaviour::readJson(j);
	opacity.fromJson(j["op"]);
}

void NormalObjectBehaviour::writeJson(json& j)
{
	j["shape"] = shape.id;
	AnimateableObjectBehaviour::writeJson(j);
	j["op"] = opacity.toJson();
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
