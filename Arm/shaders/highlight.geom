#version 430

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

void drawLine(int id1, int id2) {
	gl_Position = gl_in[id1].gl_Position;
	EmitVertex();

	gl_Position = gl_in[id2].gl_Position;
	EmitVertex();

	EndPrimitive();
}

void main() {
	drawLine(0,1);
	drawLine(1,2);
	drawLine(0,2);
}