#version 440

layout(location = 0) out vec4 fragColor;

layout(location = 1) uniform sampler2D sdfTexture;

in vec2 TexCoord;

void main() {
	vec3 sdf = texture(sdfTexture, TexCoord).rgb;
	float signedDistance = max(min(sdf.r, sdf.g), min(max(sdf.r, sdf.g), sdf.b));

	float screenPxDistance = signedDistance * 2.0 - 1.0;
	float opacity = clamp(screenPxDistance + 0.8, 0.0, 1.0);

	fragColor = mix(vec4(0.0), vec4(1.0), opacity);
}