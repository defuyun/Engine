#version 430

out vec4 fragColor;

struct Material {
	sampler2D diffuse0;
};

in vec2 texCoord;

uniform Material material;

void main() {
	fragColor = texture(material.diffuse0,  texCoord);
}