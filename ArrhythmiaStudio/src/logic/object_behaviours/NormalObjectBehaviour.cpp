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
	float t = time - baseObject->startTime;
	Transform& transform = *baseObject->node->transform;
	transform.position = glm::vec3(cos(t * 2.0f), sin(t * 2.0f), 0.0f);
}

void NormalObjectBehaviour::readJson(json& j)
{
	
}

void NormalObjectBehaviour::writeJson(json& j)
{
	
}

void NormalObjectBehaviour::drawEditor()
{
	AnimateableObjectBehaviour::drawEditor();
	ImGui::Text("The Editor is being drawn on Normal");
}
