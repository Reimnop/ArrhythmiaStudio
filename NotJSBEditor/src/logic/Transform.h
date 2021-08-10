#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;

	Transform();

	glm::mat4 getLocalMatrix() const;
};
