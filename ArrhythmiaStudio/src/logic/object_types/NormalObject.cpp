#include "NormalObject.h"

NormalObject::NormalObject(std::string name) : LevelObject(name)
{
	node = new SceneNode(name);
}

NormalObject::NormalObject(json j) : LevelObject(j["name"].get<std::string>())
{

}

NormalObject::~NormalObject() 
{
	delete node;
}

SceneNode* NormalObject::getObjectRootNode() const 
{
	return node;
}

void NormalObject::update(float time) 
{

}

void NormalObject::setObjectState(bool active)
{
	node->setActive(active);
}