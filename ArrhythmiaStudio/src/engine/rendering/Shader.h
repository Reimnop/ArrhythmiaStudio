#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>

class Shader
{
public:
	Shader(std::filesystem::path vertPath, std::filesystem::path fragPath);
	~Shader();

	uint32_t getHandle() const;
	int getAttribLocation(std::string attribName);
private:
	uint32_t handle;

	std::unordered_map<std::string, int> uniformLocations;
	std::string readAllText(std::filesystem::path path);
};
