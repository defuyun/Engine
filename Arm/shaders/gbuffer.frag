#version 430

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

struct Material {
	sampler2D diffuse0;
	sampler2D specular0;
	sampler2D normal0;
	float shininess;
};

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	
	mat3 tbn;
} fs_in;

uniform Material material;

void main() {
	gPosition = fs_in.fragPos;
	gNormal = fs_in.tbn * normalize((texture(material.normal0, fs_in.texCoord).rgb * 2) - 1).rgb;
	gAlbedoSpec.rgb = texture(material.diffuse0, fs_in.texCoord).rgb;
	gAlbedoSpec.a = texture(material.specular0, fs_in.texCoord).r;
}