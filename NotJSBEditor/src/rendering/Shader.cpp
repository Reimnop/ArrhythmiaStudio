#include "Shader.h"

Shader::Shader(const char* vertPath, const char* fragPath) {
	// Read vertex shader source
	std::ifstream vertStream(vertPath);
	std::string vertSource((std::istreambuf_iterator<char>(vertStream)), std::istreambuf_iterator<char>());
	vertStream.close();

	// Read fragment shader source
	std::ifstream fragStream(fragPath);
	std::string fragSource((std::istreambuf_iterator<char>(fragStream)), std::istreambuf_iterator<char>());
	fragStream.close();

	const char* vertSourcePtr = vertSource.c_str();
	const char* fragSourcePtr = fragSource.c_str();

	int vertSize = vertSource.size();
	int fragSize = fragSource.size();

	char infoLogBuf[1024];

	// Create vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertSourcePtr, &vertSize);
	glCompileShader(vertexShader);

	// Detect compilation errors
	int vertCode;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertCode);
	if (vertCode != GL_TRUE) {
		glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLogBuf);
		throw "Error occurred whilst compiling Shader " + std::to_string(vertexShader) + "\n\n" + infoLogBuf;
	}

	// Create fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragSourcePtr, &fragSize);
	glCompileShader(fragmentShader);

	// Detect compilation errors
	int fragCode;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragCode);
	if (fragCode != GL_TRUE) {
		glGetShaderInfoLog(fragmentShader, 1024, nullptr, infoLogBuf);
		throw "Error occurred whilst compiling Shader " + std::to_string(vertexShader) + "\n\n" + infoLogBuf;
	}

	// Create program and link
	handle = glCreateProgram();
	glAttachShader(handle, vertexShader);
	glAttachShader(handle, fragmentShader);
	glLinkProgram(handle);

	// Detect linking error
	int linkCode;
	glGetProgramiv(handle, GL_LINK_STATUS, &linkCode);
	if (linkCode != GL_TRUE) {
		throw "Error occurred whilst linking Program " + std::to_string(handle);
	}

	// Cleanup
	glDetachShader(handle, vertexShader);
	glDetachShader(handle, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Get uniform locations
	int numberOfUniforms;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numberOfUniforms);

	char uniformNameBuf[64];

	for (int i = 0; i < numberOfUniforms; i++) {
		int length;
		int size;
		GLenum type;
		glGetActiveUniform(handle, i, sizeof(uniformNameBuf), &length, &size, &type, uniformNameBuf);
		int location = glGetUniformLocation(handle, uniformNameBuf);
		uniformLocations[uniformNameBuf] = location;
	}
}

Shader::~Shader() {
	glDeleteProgram(handle);
}

uint32_t Shader::getHandle() {
	return handle;
}

int Shader::getAttribLocation(const char* attribName) {
	return uniformLocations[attribName];
}