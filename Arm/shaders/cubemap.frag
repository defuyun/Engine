#version 430

out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube cubemap;

void main() {
	fragColor = texture(cubemap, texCoord);
}