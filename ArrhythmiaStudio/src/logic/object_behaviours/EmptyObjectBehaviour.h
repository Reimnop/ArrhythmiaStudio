#pragma once

#include "AnimateableObjectBehaviour.h"

class EmptyObjectBehaviour : public AnimateableObjectBehaviour
{
public:
	EmptyObjectBehaviour(LevelObject* baseObject);
	~EmptyObjectBehaviour() override = default;

	static LevelObjectBehaviour* create(LevelObject* object);
};