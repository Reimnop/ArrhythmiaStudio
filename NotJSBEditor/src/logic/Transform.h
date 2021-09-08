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

	bool transformChanged() const;
	glm::mat4 getLocalMatrix();
private:
	glm::mat4 localMatrix;

	glm::vec3 lastPosition;
	glm::vec3 lastScale;
	glm::quat lastRotation;

	void calculateLocalMatrix();
};
