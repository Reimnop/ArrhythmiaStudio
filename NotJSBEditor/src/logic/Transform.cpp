#include "Transform.h"

Transform::Transform()
{
	position = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
	rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

glm::mat4 Transform::getLocalMatrix() const
{
	return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
}
