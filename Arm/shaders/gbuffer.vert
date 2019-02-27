#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (std140, binding = 1) uniform Matrix {
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform vec3 lightDir;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
		
	mat3 tbn;
} vs_out;

void main() {
	mat3 normalMatrix = mat3(transpose(inverse(model)));

	vs_out.fragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.normal = normalize(normalMatrix * aNormal);
	vs_out.texCoord = aTexCoord;

	vec3 tangent = normalMatrix * aTangent;
	tangent = normalize(tangent - dot(tangent, vs_out.normal) * vs_out.normal);
	vec3 bitangent = normalize(cross(tangent, vs_out.normal));
	
	vs_out.tbn = mat3(tangent, bitangent, vs_out.normal);

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}