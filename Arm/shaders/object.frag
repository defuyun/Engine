#version 430 core

out vec4 fragColor;

in VS_OUT {
	vec3 normal;
	vec3 fragPos;
	vec2 texCoord;
} vs_in;

struct DirectionLight {
	vec3 diffuse; // 12 0
	vec3 specular; // 12 12
	vec3 ambient; // 12 24
	vec3 direction; // 12 36
}; // 48

layout (std140, binding = 2) uniform LightBlock {
	DirectionLight directionLight;
};

struct Material {
	sampler2D diffuse0;
	sampler2D specular0;
	float shininess;
};

uniform Material material;
uniform vec3 camPos;

vec3 calcDirectionLight(DirectionLight directionLight) {
	vec3 norm = normalize(vs_in.normal);
	vec3 lightDir = normalize(directionLight.direction);
	float diffuseFactor = max(dot(lightDir, norm),0.0);

	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 eyeDir = normalize(camPos - vs_in.fragPos);
	float specularFactor = pow(max(dot(eyeDir, reflectDir),0), material.shininess);
	

	vec3 ambient = directionLight.ambient * texture(material.diffuse0, vs_in.texCoord).rgb;
	vec3 diffuse = directionLight.diffuse * texture(material.diffuse0, vs_in.texCoord).rgb;
	vec3 specular = directionLight.specular * texture(material.specular0, vs_in.texCoord).rgb;

	return ambient + diffuse + specular;
}
void main() {
	fragColor = vec4(calcDirectionLight(directionLight), 1.0f);
}