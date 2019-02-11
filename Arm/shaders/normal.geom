#version 430

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

float length = .1f;

in VS_OUT {
	vec3 normal;
} gs_in[];

out vec3 color;

void drawLine(int id) {
	gl_Position = gl_in[id].gl_Position;
	EmitVertex();

	gl_Position = gl_in[id].gl_Position + vec4(length * gs_in[id].normal, 0.0f);
	EmitVertex();

	EndPrimitive();
}

void drawLine2(vec4 midpoint, int index) {
	vec3 _color = vec3(0.0f);
	_color[index] = 1.0f;

	vec4 start = index > 0 ? length * index * vec4(gs_in[index - 1].normal, 0.0f) + midpoint : midpoint;
	vec4 end = length * (index+1) * vec4(gs_in[index].normal, 0.0f) + start;
	
	color = _color;
	gl_Position = start;
	EmitVertex();

	color = _color;
	gl_Position = end;
	EmitVertex();
}


void main() {
	vec4 midPointA = 0.5 * (gl_in[0].gl_Position + gl_in[1].gl_Position);
	vec4 midPointC = 0.5 * (midPointA + gl_in[2].gl_Position);
	
	drawLine2(midPointC, 0);
	drawLine2(midPointC, 1);
	drawLine2(midPointC, 2);
}