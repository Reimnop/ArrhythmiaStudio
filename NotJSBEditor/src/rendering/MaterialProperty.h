#pragma once

#include "MaterialPropertyType.h"

struct MaterialProperty
{
	const char* name;
	MaterialPropertyType type;
	int size;

	MaterialProperty(const char* name, MaterialPropertyType type, int size)
	{
		this->name = name;
		this->type = type;
		this->size = size;
	}
};
