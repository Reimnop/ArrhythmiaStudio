#pragma once

#include <string>

#include "MaterialPropertyType.h"

struct MaterialProperty {
	const char* name;
	MaterialPropertyType propertyType;
	int size;
};