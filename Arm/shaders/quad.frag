#version 430

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D tex;
uniform bool useGamma;
uniform bool useHDR;

uniform float exposure;

void main() {
	vec4 texSample = texture(tex, texCoord);
	vec3 color = vec3(texSample);

	if (useHDR)
		color = vec3(1.0f) - exp(-exposure * color);
	
	if (useGamma)
		color = pow(color, vec3(1/2.2));

	fragColor = vec4(color, texSample.w);
}