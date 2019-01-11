#version 430 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;

	vec3 position;
};

uniform Material material;
uniform Light light;
uniform vec3 camPos;

void main() {
	vec3 norm = normalize(normal);

	vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;
	vec3 diffuse = max(dot(normalize(light.position - fragPos), norm), 0) * texture(material.diffuse, texCoord).rgb * light.diffuse;
	vec3 specular = pow(max(dot(reflect(normalize(fragPos - light.position), norm), normalize(camPos - fragPos)), 0), material.shininess) * texture(material.specular, texCoord).rgb * light.specular;

	fragColor = vec4(ambient + diffuse + specular, 1.0);
}