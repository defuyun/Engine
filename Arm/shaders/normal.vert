#version 430

layout ( location = 0) in vec3 aPos;
layout ( location = 1) in vec3 aNormal;

out VS_OUT {
	vec3 normal;
} vs_out;

layout (std140, binding = 1) uniform Matrices {
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;

void main() {
	vs_out.normal =  normalize(vec3(projection * vec4(mat3(transpose(inverse(view * model))) * aNormal, 1.0f)));
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}