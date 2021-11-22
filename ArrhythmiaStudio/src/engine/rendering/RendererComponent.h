#pragma once

#include "BatchedDrawData.h"
#include "RenderCommand.h"

class RendererComponent
{
public:
	virtual ~RendererComponent() = default;

	virtual bool tryRender(glm::mat4 transform, RenderCommand** command) = 0;
};
