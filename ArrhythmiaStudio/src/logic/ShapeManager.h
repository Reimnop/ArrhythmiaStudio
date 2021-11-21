#pragma once

#include <filesystem>
#include <vector>

#include "Shape.h"

class ShapeManager
{
public:
	std::vector<Shape> shapes;

	ShapeManager();
	~ShapeManager();
private:
	Shape getShapeFromFile(std::filesystem::path path) const;
};
