#pragma once
#include "glm/glm.hpp"

class Drawer
{
public:
	virtual ~Drawer() = default;
	virtual void draw(glm::mat4 cameraMatrix) = 0;
};
