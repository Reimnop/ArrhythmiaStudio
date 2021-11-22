#pragma once

#include <stack>
#include <cstdint>
#include <glad/glad.h>

class FramebufferStack
{
public:
	FramebufferStack() = delete;

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
