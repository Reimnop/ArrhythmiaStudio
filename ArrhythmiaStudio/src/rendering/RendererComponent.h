#pragma once

#include "OutputDrawData.h"

class RendererComponent
{
public:
	virtual ~RendererComponent() = default;

	// Returns a bool so we know to discard when rendering if necessary
	virtual bool render(glm::mat4 transform, OutputDrawData** output) = 0;
};
