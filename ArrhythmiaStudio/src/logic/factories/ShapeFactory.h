#pragma once
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "logger.h"
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

		Logger::info("Loaded shape " + id);
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
		std::vector<glm::vec3> vertices;
		vertices.reserve(verticesCount);
		for (int i = 0; i < verticesCount; i++)
		{
			float x, y;
			s >> x >> y;
			vertices.emplace_back(x, y, 0.0f);
		}

		int indicesCount;
		s >> indicesCount;
		std::vector<uint32_t> indices;
		indices.reserve(indicesCount);
		for (int i = 0; i < indicesCount; i++)
		{
			uint32_t val;
			s >> val;
			indices.push_back(val);
		}

		s.close();

		return Shape(name, new Mesh(vertices, indices));
	}
};
