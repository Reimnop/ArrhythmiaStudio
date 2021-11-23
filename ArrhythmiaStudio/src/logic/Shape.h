#pragma once

#include <string>

#include "../engine/rendering/Mesh.h"

struct Shape
{
	std::string name;
	Mesh* mesh;

	Shape()
	{
		
	}

	Shape(std::string name, Mesh* mesh)
	{
		this->name = name;
		this->mesh = mesh;
	}
};
