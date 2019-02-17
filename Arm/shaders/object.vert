#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140, binding = 1) uniform Matrix {
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} vs_out;

void main() {
	vs_out.fragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.texCoord = aTexCoord;

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}