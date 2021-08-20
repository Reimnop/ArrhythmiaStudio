#pragma once

#include <string>
#include <unordered_map>

class ComputeShader
{
public:
	ComputeShader(const char* sourcePath);
	~ComputeShader();

	int getAttribLocation(const char* attribName);

	uint32_t getHandle() const;
private:
	uint32_t handle;

	std::unordered_map<std::string, int> uniformLocations;

	std::string readAllText(const char* path);
};