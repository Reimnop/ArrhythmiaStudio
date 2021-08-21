#include "ShapeManager.h"

#include <filesystem>
#include <fstream>

ShapeManager* ShapeManager::inst;

ShapeManager::ShapeManager()
{
	if (inst)
	{
		return;
	}

	inst = this;

	std::ifstream s("Assets/Shapes/shapes_list.txt");

	std::string line;
	while (std::getline(s, line))
	{
		shapes.push_back(getShapeFromFile("Assets/Shapes" / std::filesystem::path(line)));
	}

	s.close();
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

Shape ShapeManager::getShapeFromFile(std::filesystem::path path)
{
	std::ifstream s(path);

	std::string name;
	std::getline(s, name);

	int verticesCount;
	s >> verticesCount;
	glm::vec3* vertices = new glm::vec3[verticesCount];
	for (int i = 0; i < verticesCount; i++)
	{
		float x, y;
		s >> x >> y;
		vertices[i] = glm::vec3(x, y, 0.0f);
	}

	int indicesCount;
	s >> indicesCount;
	uint32_t* indices = new uint32_t[indicesCount];
	for (int i = 0; i < indicesCount; i++)
	{
		s >> indices[i];
	}

	s.close();

	return Shape(name, new Mesh(verticesCount, indicesCount, vertices, indices));
}
