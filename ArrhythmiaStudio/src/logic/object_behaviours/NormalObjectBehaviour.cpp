#include "NormalObjectBehaviour.h"
#include "../../engine/rendering/Shader.h"
#include "../../engine/rendering/Material.h"
#include "../../engine/rendering/MeshRenderer.h"
#include "../factories/ShapeFactory.h"
#include "imgui/imgui.h"

Shader* shader;

NormalObjectBehaviour::NormalObjectBehaviour(LevelObject* baseObject) : AnimateableObjectBehaviour(baseObject)
{
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

	MeshRenderer* renderer = new MeshRenderer();
	renderer->material = material;
	renderer->shader = shader;
	renderer->mesh = ShapeFactory::getShape("circle").mesh;

	baseObject->node->renderer = renderer;
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
}

void NormalObjectBehaviour::readJson(json& j)
{
	AnimateableObjectBehaviour::readJson(j);
}

void NormalObjectBehaviour::writeJson(json& j)
{
	AnimateableObjectBehaviour::writeJson(j);
}

void NormalObjectBehaviour::drawEditor()
{
	ImGui::Text("The Editor is being drawn on Normal");
	AnimateableObjectBehaviour::drawEditor();
}
