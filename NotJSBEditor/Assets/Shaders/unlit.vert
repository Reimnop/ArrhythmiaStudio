#version 440

layout(location = 0) in vec3 aPos;

layout(location = 0) uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(aPos, 1.0);
}