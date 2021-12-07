#include "LevelObject.h"

#include "Level.h"
#include "GameManager.h"
#include "utils.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "factories/ObjectBehaviourFactory.h"

LevelObject::LevelObject(std::string type, Level* level)
{
	name = "New object";
	this->type = type;
	id = Utils::randomId();
	startTime = 0.0f;
	endTime = 5.0f;
	bin = 0;
	this->level = level;
	node = new SceneNode(name);
	node->setActive(false);
	ObjectBehaviourInfo info = ObjectBehaviourFactory::getFromId(type);
	behaviour = info.createFunction(this);
}

LevelObject::LevelObject(json j)
{
	name = j["name"].get<std::string>();
	type = j["type"].get<std::string>();
	node = new SceneNode(name);
	node->setActive(false);
	ObjectBehaviourInfo info = ObjectBehaviourFactory::getFromId(type);
	behaviour = info.createFunction(this);
	fromJson(j);
}

LevelObject::~LevelObject()
{
	delete node;
	delete behaviour;
}

void LevelObject::update(float time)
{
	behaviour->update(time);
}

void LevelObject::setName(std::string name)
{
	this->name = name;
	node->name = name;
}

std::string LevelObject::getName()
{
	return name;
}

void LevelObject::fromJson(json j)
{
	name = j["name"].get<std::string>();
	if (type != j["type"].get<std::string>())
	{
		throw std::runtime_error("Mismatch object type!");
	}
	id = j["id"].get<uint64_t>();
	startTime = j["start"].get<float>();
	endTime = j["end"].get<float>();
	behaviour->readJson(j);
}

json LevelObject::toJson()
{
	json j;
	j["name"] = name;
	j["type"] = type;
	j["id"] = id;
	j["start"] = startTime;
	j["end"] = endTime;
	behaviour->writeJson(j);
	return j;
}

void LevelObject::drawEditor()
{
	ImGui::InputText("Name", &name);
	ImGui::DragFloat("Start time", &startTime, 0.1f);
	if (ImGui::IsItemEdited())
	{
		level->removeActivateList(this);
		level->insertActivateList(this);
		level->recalculateObjectsState();
	}
	ImGui::DragFloat("End time", &endTime, 0.1f);
	if (ImGui::IsItemEdited())
	{
		level->removeDeactivateList(this);
		level->insertDeactivateList(this);
		level->recalculateObjectsState();
	}

	behaviour->drawEditor();

	update(level->time);
}
