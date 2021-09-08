#version 440

layout(location = 0) in vec3 aPos;

layout(location = 0) uniform mat4 viewProjection;
layout(location = 1) uniform mat4 model;

void main() {
	gl_Position = viewProjection * model * vec4(aPos, 1.0);
}