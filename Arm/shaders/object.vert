#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoord;

layout (std140, binding = 1) uniform Matrix {
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}