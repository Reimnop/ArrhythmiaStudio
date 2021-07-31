#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera {
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	float orthographicScale = 10.0f;
	
	float depthNear = -64.0f;
	float depthFar = 64.0f;

	void calculateViewProjection(float aspect, glm::mat4* view, glm::mat4* projection);
private:
	glm::mat4 calculateModelMatrix();
};