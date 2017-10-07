#version 430 

in vec2 texCoord;
uniform sampler2D testTexture;
out vec4 color;

void main() {
	// color = texture(testTexture, texCoord);
	color = vec4(1.0f);
}