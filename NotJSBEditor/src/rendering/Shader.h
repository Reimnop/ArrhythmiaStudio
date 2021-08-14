#pragma once

#include <unordered_map>
#include <string>

class Shader
{
public:
	Shader(const char* vertPath, const char* fragPath);
	~Shader();

	uint32_t getHandle() const;
	int getAttribLocation(const char* attribName);
private:
	uint32_t handle;

	std::unordered_map<std::string, int> uniformLocations;
	std::string readAllText(const char* path);
};
