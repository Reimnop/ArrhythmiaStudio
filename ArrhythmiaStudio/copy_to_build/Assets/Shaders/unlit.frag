#version 440

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec4 frag_Color;

void main() {
    fragColor = frag_Color;
}