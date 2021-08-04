#include "Material.h"

#define MAT_GETTER(t_name, type) type Material::get##t_name(const char* name) { return *(type*)materialData[propertyIndices[name]]; }
#define MAT_SETTER(t_name, type) void Material::set##t_name(const char* name, type value) { materialData[propertyIndices[name]] = (void*)&value; }

Material::Material(Shader* shader, int propertyCount, MaterialProperty* properties)
{
	this->shader = shader;
	this->propertyCount = propertyCount;
	materialProperties = properties;

	// Calculate buffer size
	bufferSize = 0;
	for (int i = 0; i < propertyCount; i++)
	{
		bufferSize += properties[i].size;
	}

	// Initialize arrays
	materialData = new void*[propertyCount];

	for (int i = 0; i < propertyCount; i++)
	{
		propertyIndices[properties[i].name] = i;
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

	for (int i = 0; i < propertyCount; i++)
	{
		delete materialData[i];
	}
	delete[] materialData;
}

Shader* Material::getShader()
{
	return shader;
}

int Material::getUniformBuffer()
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
void structureToPointer(T* valuePointer, unsigned char* pointer)
{
	int size = sizeof(T);
	unsigned char* ptr = (unsigned char*)valuePointer;

	for (int i = 0; i < size; i++)
	{
		pointer[i] = ptr[i];
	}
}

void Material::updateBuffer()
{
	unsigned char* buffer = new unsigned char[bufferSize];
	memset(buffer, 0, bufferSize);

	int offset = 0;
	for (int i = 0; i < propertyCount; i++)
	{
		MaterialProperty property = materialProperties[i];
		switch (property.propertyType)
		{
		case MaterialPropertyType_Int:
			structureToPointer((int*)materialData[i], buffer + offset);
			break;
		case MaterialPropertyType_Float:
			structureToPointer((float*)materialData[i], buffer + offset);
			break;
		case MaterialPropertyType_Vector2:
			structureToPointer((glm::vec2*)materialData[i], buffer + offset);
			break;
		case MaterialPropertyType_Vector3:
			structureToPointer((glm::vec3*)materialData[i], buffer + offset);
			break;
		case MaterialPropertyType_Vector4:
			structureToPointer((glm::vec4*)materialData[i], buffer + offset);
			break;
		case MaterialPropertyType_Matrix2:
			structureToPointer((glm::mat2*)materialData[i], buffer + offset);
			break;
		case MaterialPropertyType_Matrix3:
			structureToPointer((glm::mat3*)materialData[i], buffer + offset);
			break;
		case MaterialPropertyType_Matrix4:
			structureToPointer((glm::mat4*)materialData[i], buffer + offset);
			break;
		}

		offset += property.size;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	delete[] buffer;
}
