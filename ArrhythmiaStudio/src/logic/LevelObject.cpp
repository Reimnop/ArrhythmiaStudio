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

LevelObject::LevelObject(json j, Level* level)
{
	name = j["name"].get<std::string>();
	type = j["type"].get<std::string>();
	id = j["id"].get<uint64_t>();
	if (j.contains("parent")) 
	{
		parentIdToInitialize = j["parent"].get<uint64_t>();
	}
	startTime = j["start"].get<float>();
	endTime = j["end"].get<float>();
	bin = j["bin"].get<int>();
	this->level = level;
	node = new SceneNode(name);
	node->setActive(false);
	ObjectBehaviourInfo info = ObjectBehaviourFactory::getFromId(type);
	behaviour = info.createFunction(this);
	behaviour->readJson(j);
}

LevelObject::~LevelObject()
{
	// Remove from parent
	if (parent)
	{
		parent->children.erase(this);
	}

	std::vector _children(children.begin(), children.end());

	// Unparent all children
	for (size_t i = 0; i < _children.size(); i++)
	{
		_children[i]->setParent(nullptr);
	}

	delete node;
	delete behaviour;
}

void LevelObject::initializeParent()
{
	if (parentIdToInitialize) 
	{
		setParent(level->levelObjects[parentIdToInitialize]);
	}
}

void LevelObject::update(float time)
{
	behaviour->update(time);
}

LevelObject* LevelObject::getParent()
{
	return parent;
}

void LevelObject::setParent(LevelObject* newParent)
{
	// Remove from old parent
	if (parent)
	{
		parent->children.erase(this);
	}

	// Add to new parent
	if (newParent)
	{
		newParent->children.emplace(this);
		parent = newParent;
		node->setParent(newParent->node);
	}
	else
	{
		parent = nullptr;
		node->setParent(nullptr);
	}
}

void LevelObject::setName(std::string& name)
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
	setParent(level->levelObjects[j["parent"].get<uint64_t>()]);
	startTime = j["start"].get<float>();
	endTime = j["end"].get<float>();
	bin = j["bin"].get<int>();
	behaviour->readJson(j);
}

json LevelObject::toJson()
{
	json j;
	j["name"] = name;
	j["type"] = type;
	j["id"] = id;
	if (parent) 
	{
		j["parent"] = parent->id;
	}
	j["start"] = startTime;
	j["end"] = endTime;
	j["bin"] = bin;
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
	ImGui::SliderInt("Editor bin", &bin, 0, 14);

	// Parent select popup
	if (ImGui::BeginPopupModal("Select parent", &parentSearchOpen))
	{
		ImGui::InputText("Search", &parentSearchStr);

		if (ImGui::IsItemEdited())
		{
			parentSearchRegex = std::regex(parentSearchStr);
		}

		if (ImGui::BeginChild("##parent-objects", ImVec2(0.0f, 0.0f), true))
		{
			for (auto &[id, obj] : level->levelObjects)
			{
				if (obj == this)
				{
					continue;
				}

				std::string title = obj->getName();

				if (std::regex_search(title, parentSearchRegex))
				{
					if (ImGui::Selectable(title.c_str()))
					{
						setParent(obj);
						ImGui::CloseCurrentPopup();
					}
				}
			}
		}
		ImGui::EndChild();

		ImGui::EndPopup();
	}

	std::string parentBtnStr = parent ? parent->getName() : "Select parent";
	if (ImGui::Button(parentBtnStr.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)))
	{
		parentSearchOpen = true;
		ImGui::OpenPopup("Select parent");
	}

	behaviour->drawEditor();
}
