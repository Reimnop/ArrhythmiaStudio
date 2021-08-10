#pragma once

#include <stack>
#include <cstdint>
#include <glad/glad.h>

static class FramebufferStack
{
public:
	static void push(uint32_t framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		framebufferStack.push(framebuffer);
	}

	static void pop()
	{
		framebufferStack.pop();

		if (framebufferStack.empty())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebufferStack.top());
		}
	}

private:
	static inline std::stack<uint32_t> framebufferStack;
};
