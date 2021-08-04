#pragma once

#include <tuple>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "MaterialProperty.h"

#define MAT_GETTER_TEMP(t_name, type) type get##t_name(const char* name);
#define MAT_SETTER_TEMP(t_name, type) void set##t_name(const char* name, type value);

class Material
{
public:
	Material(Shader* shader, int propertyCount, MaterialProperty* properties);
	~Material();

	Shader* getShader();
	int getUniformBuffer();

	MAT_GETTER_TEMP(Int, int)
	MAT_GETTER_TEMP(Float, float)
	MAT_GETTER_TEMP(Vec2, glm::vec2)
	MAT_GETTER_TEMP(Vec3, glm::vec3)
	MAT_GETTER_TEMP(Vec4, glm::vec4)
	MAT_GETTER_TEMP(Mat2, glm::mat2)
	MAT_GETTER_TEMP(Mat3, glm::mat3)
	MAT_GETTER_TEMP(Mat4, glm::mat4)

	MAT_SETTER_TEMP(Int, int)
	MAT_SETTER_TEMP(Float, float)
	MAT_SETTER_TEMP(Vec2, glm::vec2)
	MAT_SETTER_TEMP(Vec3, glm::vec3)
	MAT_SETTER_TEMP(Vec4, glm::vec4)
	MAT_SETTER_TEMP(Mat2, glm::mat2)
	MAT_SETTER_TEMP(Mat3, glm::mat3)
	MAT_SETTER_TEMP(Mat4, glm::mat4)

	void updateBuffer();
private:
	Shader* shader;
	uint32_t uniformBuffer;
	int bufferSize;

	int propertyCount;

	MaterialProperty* materialProperties;
	std::unordered_map<std::string, int> propertyIndices;
	void** materialData;
};
