#pragma once
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "../Shape.h"
#include "glm/vec3.hpp"

class ShapeFactory
{
public:
	ShapeFactory() = delete;

	static std::vector<std::string> getShapeIds()
	{
		std::vector<std::string> ids;
		ids.reserve(shapes.size());
		for (auto kv : shapes)
		{
			ids.push_back(kv.first);
		}
		return ids;
	}

	static void registerShape(std::filesystem::path path, std::string id)
	{
		Shape shape = getShapeFromFile(path);
		shape.id = id;
		shapes[id] = shape;
	}

	static Shape getShape(std::string id)
	{
		return shapes[id];
	}
private:
	static inline std::unordered_map<std::string, Shape> shapes;

	static Shape getShapeFromFile(std::filesystem::path& path)
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
};
