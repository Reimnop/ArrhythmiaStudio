#pragma once

#include <cstdint>
#include <glad/glad.h>

class Texture2D
{
public:
	Texture2D(int width, int height, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
	~Texture2D();

	void resize(int width, int height);

	uint32_t getHandle() const;
private:
	uint32_t handle;

	GLenum internalFormat;
	GLenum format;
	GLenum type;
};
