#version 430

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 texCoord;

void main() {
	vec4 pos = projection * view * vec4(aPos, 1.0f);
	texCoord = aPos;
	gl_Position = pos.xyww;
}