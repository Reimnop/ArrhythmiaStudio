#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <fstream>
#include <streambuf>
#include <string>

class Shader
{
public:
	Shader(const char* vertPath, const char* fragPath);
	~Shader();

	uint32_t getHandle();
	int getAttribLocation(const char* attribName);
private:
	uint32_t handle;

	std::unordered_map<std::string, int> uniformLocations;
};