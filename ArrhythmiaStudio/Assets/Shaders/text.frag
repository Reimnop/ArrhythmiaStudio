#version 440

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform Material {
	layout(offset = 0) vec3 color;
};

layout(location = 1) uniform float opacity = 1.0;

layout(location = 2) uniform sampler2D sdfTexture;

in vec2 TexCoord;

void main() {
	vec3 sdf = texture(sdfTexture, TexCoord).rgb;
	float signedDistance = max(min(sdf.r, sdf.g), min(max(sdf.r, sdf.g), sdf.b));

	float screenPxDistance = signedDistance * 2.0 - 1.0;
	float sdfOpacity = clamp(screenPxDistance + 0.8, 0.0, 1.0);

	fragColor = mix(vec4(0.0), vec4(color, 1.0), sdfOpacity * opacity);
}