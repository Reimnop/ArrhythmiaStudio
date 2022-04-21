#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUv;

layout(location = 0) uniform mat4 mvp;

layout(location = 0) out vec2 frag_UV;

void main() {
    frag_UV = aUv;
    gl_Position = mvp * vec4(aPos, 1.0);
}