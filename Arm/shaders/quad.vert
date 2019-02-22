#version 430

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;

void main() {
	gl_Position = model * vec4(aPos, 0.0f, 1.0f);
	texCoord = aTexCoord;
}