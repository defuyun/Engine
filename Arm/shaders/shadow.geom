#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowView[6];
uniform mat4 shadowProjection;

void main() {
	for(int face = 0; face < 6; ++face) {
		gl_Layer = face;
		for(int i = 0; i < 3; i++) {
			gl_Position = shadowProjection * shadowView[face] * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}