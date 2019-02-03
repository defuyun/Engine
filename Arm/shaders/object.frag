#version 430 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

struct Material {
	sampler2D diffuse0;
	sampler2D specular0;
	float shininess;
};

struct DirectionLight {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;

	vec3 direction;
};

uniform Material material;
uniform DirectionLight directionLight;
uniform vec3 camPos;

uniform samplerCube cubemap;

vec3 calcDirectionLight(DirectionLight directionLight) {
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-directionLight.direction);
	float diffuseFactor = max(dot(lightDir, norm),0.0);

	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 eyeDir = normalize(camPos - fragPos);
	float specularFactor = pow(max(dot(eyeDir, reflectDir),0), material.shininess);
	

	vec3 ambient = directionLight.ambient * texture(material.diffuse0, texCoord).rgb;
	vec3 diffuse = directionLight.diffuse * texture(material.diffuse0, texCoord).rgb;
	vec3 specular = directionLight.specular * texture(material.specular0, texCoord).rgb;

	return ambient + diffuse + specular;
}

vec3 calReflection() {
	vec3 norm = normalize(normal);
	vec3 eyeDir = normalize(camPos - fragPos);

	vec3 reflectDir = normalize(reflect(-eyeDir, norm));
	return vec3(texture(cubemap, reflectDir));
}

vec3 calRefract() {
	vec3 norm = normalize(normal);
	vec3 eyeDir = normalize(camPos - fragPos);

	vec3 refractDir = normalize(refract(-eyeDir, norm, 0.6));
	return vec3(texture(cubemap, refractDir));
}

void main() {
	vec3 result = vec3(0.0,0.0,0.0);
	result += calcDirectionLight(directionLight);
	result += calReflection();
	result += calRefract();

	fragColor = vec4(result, 1.0f);	
}