#include "Camera.h"

void Camera::calculateViewProjection(float aspect, glm::mat4* view, glm::mat4* projection)
{
	float leftRight = orthographicScale * aspect * 0.5f;
	float bottomTop = orthographicScale * 0.5f;

	(*view) = glm::inverse(calculateModelMatrix());
	(*projection) = glm::orthoRH(-leftRight, leftRight, -bottomTop, bottomTop, depthNear, depthFar);
}

glm::mat4 Camera::calculateModelMatrix()
{
	return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation);
}
