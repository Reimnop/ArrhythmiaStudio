#version 440

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform Transform {
	layout(offset = 0) mat4 model;
	layout(offset = 64) mat4 view;
	layout(offset = 128) mat4 projection;
	layout(offset = 192) mat4 modelViewProjection;
};

void main() {
	gl_Position = modelViewProjection * vec4(aPos, 1.0);
}