#version 430 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

struct DirectionLight {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;

	vec3 direction;
};

struct Material {
	float shininess;
	sampler2D diffuse0;
	vec3 specular;
};

uniform DirectionLight directionLight;
uniform Material material;
uniform vec3 camPos;

vec3 calcDirectionLight(DirectionLight directionLight) {
	vec3 ld = normalize(-directionLight.direction);
	vec3 nm = normalize(normal);
	float df = max(dot(ld, nm), 0);

	vec3 eyeDir = normalize(camPos - fragPos);
	vec3 rld = reflect(-ld, nm);
	float sf = pow(max(dot(eyeDir, rld),0), material.shininess);
	
	vec3 ambient = directionLight.ambient * texture(material.diffuse0, texCoord).rgb;
	vec3 diffuse = df * directionLight.diffuse * texture(material.diffuse0, texCoord).rgb;
	vec3 specular = sf * directionLight.specular * texture(material.diffuse0, texCoord).rgb;

	return ambient + diffuse + specular;
}

void main() {
	fragColor = vec4(calcDirectionLight(directionLight), 1.0f);
	// fragColor = vec4(directionLight.direction, 1.0f);
}