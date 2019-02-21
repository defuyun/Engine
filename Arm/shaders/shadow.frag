#version 430

uniform float near;
uniform float far;
uniform vec3 lightPos;

in vec3 fragPos;

void main() {
	gl_FragDepth = length(fragPos - lightPos) / (far - near);
}
