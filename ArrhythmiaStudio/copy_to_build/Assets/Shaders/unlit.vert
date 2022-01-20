#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

layout(location = 0) uniform mat4 mvp;

layout(location = 0) out vec4 frag_Color;

void main() {
    frag_Color = aColor;
    gl_Position = mvp * vec4(aPos, 1.0);
}