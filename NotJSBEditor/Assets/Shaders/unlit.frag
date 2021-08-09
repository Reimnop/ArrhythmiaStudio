#version 440

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 1) uniform Material {
	layout(offset = 0) vec3 color;
};

void main() {
	fragColor = vec4(color, 1.0);
}