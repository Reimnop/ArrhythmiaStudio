#pragma once

#include <vector>
#include <any>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

#include "MaterialProperty.h"

#define MAT_GETTER_TEMP(t_name, type) type get##t_name(const char* name);
#define MAT_SETTER_TEMP(t_name, type) void set##t_name(const char* name, type value);

class Material
{
public:
	Material(int propertyCount, MaterialProperty* properties);
	~Material();

	int getUniformBuffer() const;

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
	uint32_t uniformBuffer;
	int bufferSize;

	int propertyCount;

	std::vector<MaterialProperty> materialProperties;
	std::vector<std::any> materialData;
	std::unordered_map<std::string, int> propertyIndices;
};
