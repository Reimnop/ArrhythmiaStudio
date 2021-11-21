#include "LevelObject.h"
#include "utils.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "object_behaviours/LevelObjectBehaviour.h"
#include "object_behaviours/NormalObjectBehaviour.h"

LevelObject::LevelObject()
{
	name = "New object";
	id = Utils::randomId();
	startTime = 0.0f;
	endTime = 5.0f;
	node = new SceneNode(name);
	node->setActive(false);
	initializeObjectBehaviour<NormalObjectBehaviour>();
}

LevelObject::~LevelObject()
{
	delete node;
	delete behaviour;
}

void LevelObject::setName(std::string name)
{
	this->name = name;
}

std::string LevelObject::getName()
{
	return name;
}

void LevelObject::fromJson(json j)
{
	name = j["name"].get<std::string>();
	id = j["id"].get<uint64_t>();
	startTime = j["start"].get<float>();
	endTime = j["end"].get<float>();
	node = new SceneNode(name);
	behaviour->fromJson(j["data"]);
}

json LevelObject::toJson()
{
	json j;
	j["name"] = name;
	j["id"] = id;
	j["start"] = startTime;
	j["end"] = endTime;
	json data;
	behaviour->toJson(data);
	j["data"] = data;
	return j;
}

void LevelObject::drawEditor()
{
	ImGui::InputText("Name", &name);

	behaviour->drawEditor();
}
