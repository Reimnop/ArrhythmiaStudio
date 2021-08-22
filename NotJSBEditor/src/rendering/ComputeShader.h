#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

class ComputeShader
{
public:
	ComputeShader(std::filesystem::path sourcePath);
	~ComputeShader();

	int getAttribLocation(std::string attribName);

	uint32_t getHandle() const;
private:
	uint32_t handle;

	std::unordered_map<std::string, int> uniformLocations;

	std::string readAllText(std::filesystem::path path);
};
