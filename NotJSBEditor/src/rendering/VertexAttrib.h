#pragma once

#include <glad/glad.h>

struct VertexAttrib
{
	GLenum pointerType;
	int size;
	int sizeInBytes;
	bool normalized;
};
