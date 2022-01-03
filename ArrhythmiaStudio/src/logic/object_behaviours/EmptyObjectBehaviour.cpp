#include "EmptyObjectBehaviour.h"

EmptyObjectBehaviour::EmptyObjectBehaviour(LevelObject* baseObject) : AnimateableObjectBehaviour(baseObject)
{
}

LevelObjectBehaviour* EmptyObjectBehaviour::create(LevelObject* object)
{
	return new EmptyObjectBehaviour(object);
}
