#pragma once

#include "DrawCommand.h"

// glDrawElements
struct DrawElementsCommand : DrawCommand
{
	int count;
	int offset;
};
