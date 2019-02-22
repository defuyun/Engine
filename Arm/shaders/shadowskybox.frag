#version 430

out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube shadowMap;

void main() {
	float depthValue = texture(shadowMap, texCoord).r;
	fragColor = vec4(vec3(depthValue), 1.0f);
}