#version 430

layout ( location = 0) in vec3 aPos;
layout ( location = 1) in vec3 aNormal;
layout ( location = 2) in vec2 aTexCoord;

out VS_OUT {
	out vec3 normal;
	out vec3 fragPos;
	out vec2 texCoord;
	out vec4 fragLightPos;
} vs_out;

layout (std140, binding = 1) uniform Matrices {
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;
uniform mat4 lightMatrix;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.texCoord = aTexCoord;
	vs_out.fragLightPos = lightMatrix * vec4(vs_out.fragPos, 1.0f);
}
