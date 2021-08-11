#include "Material.h"

#include <glad/glad.h>

#define MAT_GETTER(t_name, type) type Material::get##t_name(const char* name) { return std::any_cast<type>(materialData[propertyIndices[name]]); }
#define MAT_SETTER(t_name, type) void Material::set##t_name(const char* name, type value) { materialData[propertyIndices[name]] = value; }

Material::Material(Shader* shader, int propertyCount, MaterialProperty* properties)
{
	this->shader = shader;
	this->propertyCount = propertyCount;

	for (int i = 0; i < propertyCount; i++)
	{
		materialProperties.push_back(properties[i]);
	}

	// Calculate buffer size
	bufferSize = 0;
	for (int i = 0; i < propertyCount; i++)
	{
		bufferSize += materialProperties[i].size;
	}

	// Initialize data vector
	materialData.resize(propertyCount);

	for (int i = 0; i < propertyCount; i++)
	{
		propertyIndices[materialProperties[i].name] = i;
	}

	// Create uniform buffer
	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Material::~Material()
{
	glDeleteBuffers(1, &uniformBuffer);

	propertyIndices.clear();

	materialProperties.clear();
	materialProperties.shrink_to_fit();

	materialData.clear();
	materialData.shrink_to_fit();
}

Shader* Material::getShader() const
{
	return shader;
}

int Material::getUniformBuffer() const
{
	return uniformBuffer;
}

MAT_GETTER(Int, int)
MAT_GETTER(Float, float)
MAT_GETTER(Vec2, glm::vec2)
MAT_GETTER(Vec3, glm::vec3)
MAT_GETTER(Vec4, glm::vec4)
MAT_GETTER(Mat2, glm::mat2)
MAT_GETTER(Mat3, glm::mat3)
MAT_GETTER(Mat4, glm::mat4)

MAT_SETTER(Int, int)
MAT_SETTER(Float, float)
MAT_SETTER(Vec2, glm::vec2)
MAT_SETTER(Vec3, glm::vec3)
MAT_SETTER(Vec4, glm::vec4)
MAT_SETTER(Mat2, glm::mat2)
MAT_SETTER(Mat3, glm::mat3)
MAT_SETTER(Mat4, glm::mat4)

template <typename T>
void structureToPointer(T value, uint8_t* pointer)
{
	int size = sizeof(T);
	uint8_t* ptr = (uint8_t*)&value;

	for (int i = 0; i < size; i++)
	{
		pointer[i] = ptr[i];
	}
}

void Material::updateBuffer()
{
	uint8_t* buffer = new uint8_t[bufferSize];
	memset(buffer, 0, bufferSize);

	int offset = 0;
	for (int i = 0; i < propertyCount; i++)
	{
		MaterialProperty property = materialProperties[i];
		switch (property.type)
		{
		case MaterialPropertyType_Int:
			structureToPointer(std::any_cast<int>(materialData[i]), buffer + offset);
			break;
		case MaterialPropertyType_Float:
			structureToPointer(std::any_cast<float>(materialData[i]), buffer + offset);
			break;
		case MaterialPropertyType_Vector2:
			structureToPointer(std::any_cast<glm::vec2>(materialData[i]), buffer + offset);
			break;
		case MaterialPropertyType_Vector3:
			structureToPointer(std::any_cast<glm::vec3>(materialData[i]), buffer + offset);
			break;
		case MaterialPropertyType_Vector4:
			structureToPointer(std::any_cast<glm::vec4>(materialData[i]), buffer + offset);
			break;
		case MaterialPropertyType_Matrix2:
			structureToPointer(std::any_cast<glm::mat2>(materialData[i]), buffer + offset);
			break;
		case MaterialPropertyType_Matrix3:
			structureToPointer(std::any_cast<glm::mat3>(materialData[i]), buffer + offset);
			break;
		case MaterialPropertyType_Matrix4:
			structureToPointer(std::any_cast<glm::mat4>(materialData[i]), buffer + offset);
			break;
		}

		offset += property.size;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	delete[] buffer;
}
