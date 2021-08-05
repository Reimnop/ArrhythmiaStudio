#pragma once

#include "InputDrawData.h"
#include "OutputDrawData.h"

class RendererComponent
{
public:
	virtual ~RendererComponent() = 0;

	// Returns a bool so we know to discard when rendering if necessary
	virtual bool render(InputDrawData input, OutputDrawData* output) = 0;
};
