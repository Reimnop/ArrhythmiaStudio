#version 440

layout(location = 0) out vec4 fragColor;

layout(location = 1) uniform vec3 color;
layout(location = 2) uniform sampler2D u_texture;

layout(location = 0) in vec2 frag_UV;

void main() {
    vec3 outColor = texture(u_texture, frag_UV).rgb * color;
    fragColor = vec4(outColor, 1.0);
}