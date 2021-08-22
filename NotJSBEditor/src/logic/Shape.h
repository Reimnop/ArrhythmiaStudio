#pragma once

#include <string>

#include "../rendering/Mesh.h"

struct Shape
{
	std::string name;
	Mesh* mesh;

	Shape(std::string name, Mesh* mesh)
	{
		this->name = name;
		this->mesh = mesh;
	}
};
