#version 430

in vec3 aPos;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * aNormal;
	texCoord = aTexCoord;
}
