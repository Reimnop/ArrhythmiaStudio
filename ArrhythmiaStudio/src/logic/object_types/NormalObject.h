#pragma once
#include "../LevelObject.h"
#include "json.hpp"

using namespace nlohmann;

class NormalObject : public LevelObject 
{
public:
	NormalObject(std::string name);
	NormalObject(json j);
	~NormalObject() override;

	SceneNode* getObjectRootNode() const override;

	void update(float time) override;
	void setObjectState(bool active) override;
private:
	SceneNode* node;
};