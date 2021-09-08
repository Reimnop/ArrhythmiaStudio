#version 440

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform Material {
	layout(offset = 0) vec3 color;
};

layout(location = 1) uniform float opacity = 1.0;

void main() {
	fragColor = vec4(color, opacity);
}