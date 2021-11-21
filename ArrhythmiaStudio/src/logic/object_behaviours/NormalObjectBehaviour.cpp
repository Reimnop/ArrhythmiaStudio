#include "NormalObjectBehaviour.h"
#include "../../rendering/Shader.h"
#include "../../rendering/Material.h"
#include "../../rendering/MeshRenderer.h"
#include "imgui/imgui.h"

ShapeManager* NormalObjectBehaviour::shapeManager;

NormalObjectBehaviour::NormalObjectBehaviour(LevelObject* baseObject) : AnimateableObjectBehaviour(baseObject)
{
	if (!shapeManager)
	{
		shapeManager = new ShapeManager();
	}

	MaterialProperty properties[]
	{
		MaterialProperty("Color", MaterialPropertyType_Vector3, 12)
	};

	Material* material = new Material(1, properties);
	material->setVec3("Color", glm::vec3(1.0f));
	material->updateBuffer();

	Shader* shader = new Shader("Assets/Shaders/unlit.vert", "Assets/Shaders/unlit.frag");

	MeshRenderer* renderer = new MeshRenderer();
	renderer->material = material;
	renderer->shader = shader;
	renderer->mesh = shapeManager->shapes[0].mesh;

	baseObject->node->renderer = renderer;
}

NormalObjectBehaviour::~NormalObjectBehaviour()
{
	delete baseObject->node->renderer;
}

void NormalObjectBehaviour::update(float time) 
{
	baseObject->node->transform->position = glm::vec3(cos(time), sin(time), 0.0f);
}

void NormalObjectBehaviour::fromJson(json& j)
{
	
}

void NormalObjectBehaviour::toJson(json& j)
{
	
}

void NormalObjectBehaviour::drawEditor()
{
	AnimateableObjectBehaviour::drawEditor();
	ImGui::Text("The Editor is being drawn on Normal");
}
