#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) uniform mat4 mvp;

out vec2 TexCoord;

void main() {
	TexCoord = aTexCoord;
	gl_Position = mvp * vec4(aPos, 1.0);
}