#include "ComputeShader.h"

#include <fstream>
#include <sstream>
#include <glad/glad.h>

ComputeShader::ComputeShader(const char* sourcePath)
{
	char infoLogBuf[2048];
	char uniformNameBuf[2048];

	// Get source
	std::string source = readAllText(sourcePath);

	const char* sourcePtr = source.c_str();
	int sourceLength = source.size();

	// Compile shader
	uint32_t shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &sourcePtr, &sourceLength);
	glCompileShader(shader);

	// Detect compilation errors
	int vertCode;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &vertCode);
	if (vertCode != GL_TRUE)
	{
		glGetShaderInfoLog(shader, sizeof(infoLogBuf), nullptr, infoLogBuf);
		throw std::runtime_error("Error occurred whilst compiling Shader " + std::to_string(handle) + "\n\n" + infoLogBuf);
	}

	// Create program
	handle = glCreateProgram();
	glAttachShader(handle, shader);
	glLinkProgram(handle);

	// Detect linking error
	int linkCode;
	glGetProgramiv(handle, GL_LINK_STATUS, &linkCode);
	if (linkCode != GL_TRUE)
	{
		throw std::runtime_error("Error occurred whilst linking Program " + std::to_string(handle));
	}

	// Clean up
	glDetachShader(handle, shader);
	glDeleteShader(shader);

	// Get uniform locations
	int numberOfUniforms;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numberOfUniforms);

	for (int i = 0; i < numberOfUniforms; i++)
	{
		int length;
		int size;
		GLenum type;
		glGetActiveUniform(handle, i, sizeof(uniformNameBuf), &length, &size, &type, uniformNameBuf);
		int location = glGetUniformLocation(handle, uniformNameBuf);
		uniformLocations[uniformNameBuf] = location;
	}
}

ComputeShader::~ComputeShader()
{
	glDeleteProgram(handle);
}

int ComputeShader::getAttribLocation(const char* attribName)
{
	return uniformLocations[attribName];
}

uint32_t ComputeShader::getHandle() const
{
	return handle;
}

std::string ComputeShader::readAllText(const char* path)
{
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}
