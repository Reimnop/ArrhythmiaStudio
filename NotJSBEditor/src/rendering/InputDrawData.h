#pragma once

#include <glm/glm.hpp>

// Contains all necessary data for rendering
struct InputDrawData {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 modelViewProjection;
};