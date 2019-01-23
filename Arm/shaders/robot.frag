#version 430 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

struct DirectionLight {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;

	vec3 direction;
};

struct Material {
	float shininess;
	vec3 diffuse;
	vec3 specular;
};

uniform DirectionLight directionLight;
uniform Material material;
uniform vec3 camPos;

void main() {
	fragColor = vec4(normalize(normal), 1.0);
}