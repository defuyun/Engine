#version 430

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D quadTex;

void main() {
	fragColor = texture(quadTex, texCoord);
}