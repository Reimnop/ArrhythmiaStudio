#include "Shader.h"

#include <fstream>
#include <sstream>
#include <glad/glad.h>

Shader::Shader(std::filesystem::path vertPath, std::filesystem::path fragPath)
{
	char infoLogBuf[2048];
	char uniformNameBuf[2048];

	// Read vertex shader source
	std::string vertSource = readAllText(vertPath);

	// Read fragment shader source
	std::string fragSource = readAllText(fragPath);

	const char* vertSourcePtr = vertSource.c_str();
	const char* fragSourcePtr = fragSource.c_str();

	int vertSize = vertSource.size();
	int fragSize = fragSource.size();

	// Create vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertSourcePtr, &vertSize);
	glCompileShader(vertexShader);

	// Detect compilation errors
	int vertCode;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertCode);
	if (vertCode != GL_TRUE)
	{
		glGetShaderInfoLog(vertexShader, sizeof(infoLogBuf), nullptr, infoLogBuf);
		throw std::runtime_error(
			"Error occurred whilst compiling Shader " + std::to_string(vertexShader) + "\n\n" + infoLogBuf);
	}

	// Create fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragSourcePtr, &fragSize);
	glCompileShader(fragmentShader);

	// Detect compilation errors
	int fragCode;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragCode);
	if (fragCode != GL_TRUE)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(infoLogBuf), nullptr, infoLogBuf);
		throw std::runtime_error(
			"Error occurred whilst compiling Shader " + std::to_string(vertexShader) + "\n\n" + infoLogBuf);
	}

	// Create program and link
	handle = glCreateProgram();
	glAttachShader(handle, vertexShader);
	glAttachShader(handle, fragmentShader);
	glLinkProgram(handle);

	// Detect linking error
	int linkCode;
	glGetProgramiv(handle, GL_LINK_STATUS, &linkCode);
	if (linkCode != GL_TRUE)
	{
		throw std::runtime_error("Error occurred whilst linking Program " + std::to_string(handle));
	}

	// Cleanup
	glDetachShader(handle, vertexShader);
	glDetachShader(handle, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

Shader::~Shader()
{
	glDeleteProgram(handle);
}

uint32_t Shader::getHandle() const
{
	return handle;
}

int Shader::getAttribLocation(std::string attribName)
{
	return uniformLocations[attribName];
}

std::string Shader::readAllText(std::filesystem::path path)
{
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}
