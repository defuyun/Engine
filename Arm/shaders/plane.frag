#version 430

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D tex;

void main() {
	fragColor = 1.0 - texture(tex, texCoord);
}