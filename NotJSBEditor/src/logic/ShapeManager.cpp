#include "ShapeManager.h"

#include <fstream>

ShapeManager* ShapeManager::inst;

ShapeManager::ShapeManager()
{
	if (inst)
	{
		return;
	}

	inst = this;

	shapes.push_back(getShapeFromFile("Assets/Shapes/square.shp"));
}

ShapeManager::~ShapeManager()
{
	for (Shape shape : shapes)
	{
		delete shape.mesh;
	}

	shapes.clear();
	shapes.shrink_to_fit();
}

Shape ShapeManager::getShapeFromFile(const char* path)
{
	std::ifstream stream(path);

	std::string name;
	stream >> name;

	int verticesCount;
	stream >> verticesCount;
	glm::vec3* vertices = new glm::vec3[verticesCount];
	for (int i = 0; i < verticesCount; i++)
	{
		float x, y;
		stream >> x >> y;
		vertices[i] = glm::vec3(x, y, 0.0f);
	}

	int indicesCount;
	stream >> indicesCount;
	uint32_t* indices = new uint32_t[indicesCount];
	for (int i = 0; i < indicesCount; i++)
	{
		stream >> indices[i];
	}

	stream.close();

	return Shape(name, new Mesh(verticesCount, indicesCount, vertices, indices));
}
