#version 430

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

float length = .1f;

in VS_OUT {
	vec3 normal;
} gs_in[];

void drawLine(int id) {
	gl_Position = gl_in[id].gl_Position;
	EmitVertex();

	gl_Position = gl_in[id].gl_Position + vec4(length * gs_in[id].normal, 0.0f);
	EmitVertex();

	EndPrimitive();
}

void main() {
	drawLine(0);
	drawLine(1);
	drawLine(2);
}