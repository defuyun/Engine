#version 430

layout (location = 0) in vec3 aPos;

out vec3 texCoord;

layout (std140, binding = 1) uniform Matrix {
	mat4 projection;
	mat4 view;
};

void main() {
	mat4 viewCpy = mat4(view[0], view[1], view[2], vec4(0.0f, 0.0f, 0.0f, 1.0f));

	texCoord = aPos;
	gl_Position =  (projection * viewCpy * vec4(aPos, 1.0f)).xyww;
}