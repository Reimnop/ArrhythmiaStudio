#pragma once

#include <cstdint>
#include <filesystem>
#include <glad/glad.h>

class Texture2D
{
public:
	Texture2D(int width, int height, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA,
	          GLenum type = GL_UNSIGNED_BYTE);
	Texture2D(std::filesystem::path path);
	~Texture2D();

	void resize(int width, int height);

	uint32_t getHandle() const;
	void getSize(int* width, int* height) const;
private:
	uint32_t handle;

	int width;
	int height;

	GLenum internalFormat;
	GLenum format;
	GLenum type;
};
