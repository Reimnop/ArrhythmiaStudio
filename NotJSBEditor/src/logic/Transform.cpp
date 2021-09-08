#include "Transform.h"

Transform::Transform()
{
	position = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
	rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	lastPosition = glm::vec3(0.0f);
	lastScale = glm::vec3(1.0f);
	lastRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	calculateLocalMatrix();
}

bool Transform::transformChanged() const
{
	return position != lastPosition || scale != lastScale || rotation != lastRotation;
}

glm::mat4 Transform::getLocalMatrix()
{
	if (transformChanged())
	{
		calculateLocalMatrix();

		lastPosition = position;
		lastScale = scale;
		lastRotation = rotation;
	}

	return localMatrix;
}

void Transform::calculateLocalMatrix()
{
	localMatrix = glm::translate(glm::mat4(1.0f), position)* glm::toMat4(rotation)* glm::scale(glm::mat4(1.0f), scale);
}
