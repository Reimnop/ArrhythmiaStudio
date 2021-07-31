#pragma once

#include "DrawCommand.h"

// glDrawElements
struct DrawElementsCommand : public DrawCommand {
	int count;
	int offset;
};
