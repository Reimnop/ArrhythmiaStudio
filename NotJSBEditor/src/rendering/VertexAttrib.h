#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct VertexAttrib
{
	GLenum pointerType;
	int size;
	int sizeInBytes;
	bool normalized;
};
