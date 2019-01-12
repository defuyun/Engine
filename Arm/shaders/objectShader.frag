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

struct PointLight {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;

	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
};

struct DirectionLight {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;

	vec3 direction;
};

struct SpotLight {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;

	vec3 position;
	vec3 direction;
	
	float innerCutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform DirectionLight directionLight;
uniform PointLight[4] pointLights;
uniform SpotLight spotLight;

uniform vec3 camPos;

vec3 calcPointLight(PointLight pointLight) {
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(pointLight.position - fragPos);
	float dist = length(pointLight.position - fragPos);
	float diffuseFactor = max(dot(lightDir, norm),0.0);

	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 eyeDir = normalize(camPos - fragPos);
	float specularFactor = pow(max(dot(eyeDir, reflectDir),0), material.shininess);
	
	float attenuate = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * pow(dist,2));

	vec3 ambient = pointLight.ambient * texture(material.diffuse, texCoord).rgb * attenuate;
	vec3 diffuse = pointLight.diffuse * texture(material.diffuse, texCoord).rgb * attenuate;
	vec3 specular = pointLight.specular * texture(material.specular, texCoord).rgb * attenuate;

	return ambient + diffuse + specular;
}

vec3 calcDirectionLight(DirectionLight directionLight) {
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(directionLight.direction);
	float diffuseFactor = max(dot(lightDir, norm),0.0);

	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 eyeDir = normalize(camPos - fragPos);
	float specularFactor = pow(max(dot(eyeDir, reflectDir),0), material.shininess);
	

	vec3 ambient = directionLight.ambient * texture(material.diffuse, texCoord).rgb;
	vec3 diffuse = directionLight.diffuse * texture(material.diffuse, texCoord).rgb;
	vec3 specular = directionLight.specular * texture(material.specular, texCoord).rgb;

	return ambient + diffuse + specular;
}

vec3 calcSpotLight(SpotLight spotLight) {
	vec3 lightDir = normalize(spotLight.position - fragPos);
	vec3 norm = normalize(normal);
	float dist = length(spotLight.position - fragPos);

	float diffuseFactor = max(dot(lightDir, norm),0.0);

	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 eyeDir = normalize(camPos - fragPos);
	float specularFactor = pow(max(dot(eyeDir, reflectDir),0), material.shininess);
	
	float attenuate = 1.0 / (spotLight.constant + spotLight.linear * dist + spotLight.quadratic * pow(dist,2));

	vec3 spotDir = normalize(spotLight.direction);
	lightDir = normalize(-lightDir);

	float eps = spotLight.innerCutOff - spotLight.outerCutOff;
	float spotFactor = min(max((dot(spotDir, lightDir) - spotLight.outerCutOff) / eps, 0.0),1.0);

	vec3 ambient = spotLight.ambient * texture(material.diffuse, texCoord).rgb * attenuate * spotFactor;
	vec3 diffuse = spotLight.diffuse * texture(material.diffuse, texCoord).rgb * attenuate * spotFactor;
	vec3 specular = spotLight.specular * texture(material.specular, texCoord).rgb * attenuate * spotFactor;

	return ambient + diffuse + specular;
}

void main() {
	vec3 result = vec3(0.0,0.0,0.0);
	for (int i = 0; i < 4; i++) {
		result += calcPointLight(pointLights[i]);
	}
	
	result += calcDirectionLight(directionLight);
	result += calcSpotLight(spotLight);

	fragColor = vec4(result, 1.0f);
}