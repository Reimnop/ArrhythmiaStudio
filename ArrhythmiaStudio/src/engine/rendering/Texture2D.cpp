#include "Texture2D.h"

#include <stb/stb_image.h>

Texture2D::Texture2D(int width, int height, GLenum internalFormat, GLenum format, GLenum type)
{
	this->internalFormat = internalFormat;
	this->format = format;
	this->type = type;
	this->width = width;
	this->height = height;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(std::filesystem::path path)
{
	int w, h, numChannels;
	uint8_t* data = stbi_load(path.generic_string().c_str(), &w, &h, &numChannels, 0);

	this->internalFormat = numChannels == 4 ? GL_RGBA : GL_RGB;
	this->format = numChannels == 4 ? GL_RGBA : GL_RGB;
	this->type = GL_UNSIGNED_BYTE;
	this->width = w;
	this->height = h;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}


Texture2D::~Texture2D()
{
	glDeleteTextures(1, &handle);
}

void Texture2D::resize(int width, int height)
{
	this->width = width;
	this->height = height;

	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::getSize(int* width, int* height) const
{
	*width = this->width;
	*height = this->height;
}

uint32_t Texture2D::getHandle() const
{
	return handle;
}
