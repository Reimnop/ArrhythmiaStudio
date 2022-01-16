#version 440

layout(location = 0) out vec4 fragColor;

layout(location = 1) uniform vec4 color;
layout(location = 2) uniform sampler2D sdfTexture;

layout(location = 3) uniform float width = 0.4;
layout(location = 4) uniform float edge = 0.2;

layout(location = 0) in vec2 frag_TexCoord;

void main() {
	vec3 sdf = texture(sdfTexture, frag_TexCoord).rgb;
	float dist = 1.0 - max(min(sdf.r, sdf.g), min(max(sdf.r, sdf.g), sdf.b));
	float sdfOpacity = 1.0 - smoothstep(width, width + edge, dist);

	fragColor = vec4(color.rgb, color.a * sdfOpacity);
}