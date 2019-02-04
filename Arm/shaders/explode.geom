#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float time;

vec3 getNormal() {
	vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	return normalize(cross(a,b));
}

in VS_OUT {
	vec3 normal;
	vec3 fragPos;
	vec2 texCoord;
} gs_in[];

out GS_OUT {
	vec3 normal;
	vec3 fragPos;
	vec2 texCoord;
} gs_out;

vec4 explode(int id, vec3 normal) {
	vec4 dir = (sin(time) + 1) * 0.5 * vec4(normal,1.0);
	return gl_in[id].gl_Position + dir;
}

void main() {
	vec3 norm = getNormal();
	gl_Position = explode(0, norm);
	gs_out.normal = gs_in[0].normal;
	gs_out.fragPos = gs_in[0].fragPos;
	gs_out.texCoord = gs_in[0].texCoord;

	EmitVertex();

	gl_Position = explode(1, norm);
	gs_out.normal = gs_in[1].normal;
	gs_out.fragPos = gs_in[1].fragPos;
	gs_out.texCoord = gs_in[1].texCoord;
	EmitVertex();

	gl_Position = explode(2, norm);
	gs_out.normal = gs_in[2].normal;
	gs_out.fragPos = gs_in[2].fragPos;
	gs_out.texCoord = gs_in[2].texCoord;
	EmitVertex();

	EndPrimitive();
}
