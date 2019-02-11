#version 430

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D tex;

void main() {
	float depth = texture(tex, texCoord).r; 
	fragColor = vec4(depth);
}