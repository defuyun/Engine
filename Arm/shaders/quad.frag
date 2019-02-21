#version 430

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D tex;
uniform bool gamma;

void main() {
	vec4 texSample = texture(tex, texCoord);
	vec3 color = vec3(texSample);
	
	if (gamma)
		color = pow(color, vec3(1/2.2));

	fragColor = vec4(color, texSample.w);
}