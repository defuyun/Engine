#version 430

in vec3 pos;
in vec2 tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

out vec2 texCoord;

void main() {
	gl_Position = project * view * model * vec4(pos,1.0f);
	texCoord = tex;
}