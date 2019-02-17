#version 430

out vec4 fragColor;

struct DirectionLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int type;
	vec3 direction;
};

struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int type;
	vec3 position;
	float kc;
	float kl;
	float kq;
};

struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int type;
	vec3 position;
	float kc;
	float kl;
	float kq;
	vec3 direction;
	float cutOffInner;
	float cutOffOuter;
};

struct Material {
	sampler2D diffuse0;
	sampler2D specular0;
	float shininess;
};

uniform Material material;

layout (std140, binding = 4) uniform LightBlock {
	DirectionLight directionLight;
	PointLight pointLight;
	SpotLight spotLight;
};

layout (std140, binding = 5) uniform CamPosBlock {
	vec3 camPos;
};

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
} fs_in;

// light direction in reverse incident direction : from fragPos to lightPos
float getDiffuseFactor(vec3 lightDirection, vec3 normal) {
	return max(dot(normalize(lightDirection), normalize(normal)), 0);
}

// light direction in reverse incident direction
// cam direction from fragment to camera position
float getSpecularFactor(vec3 lightDirection, vec3 camDirection, vec3 normal) {
	vec3 reflectedLight = normalize(reflect(normalize(-lightDirection), normalize(normal)));
	return pow(max(dot(normalize(camDirection), reflectedLight), 0), material.shininess);
}

float getAttenuation(float kc, float kl, float kq, float dist) {
	return 1.0 / (kc + kl * dist + kq * dist * dist);
}

vec3 calculateLightColor(vec3 diffuse, vec3 specular, vec3 ambient) {
	return diffuse * texture(material.diffuse0, fs_in.texCoord).rgb +
			specular * texture(material.specular0, fs_in.texCoord).rgb +
			ambient * texture(material.diffuse0, fs_in.texCoord).rgb;
}

vec3 calculateDirectionLight(DirectionLight light, vec3 normal) {
	float diffuseFactor = getDiffuseFactor(-light.direction, normal);
	float specularFactor = getSpecularFactor(-light.direction, camPos - fs_in.fragPos, normal);

	return calculateLightColor(diffuseFactor * light.diffuse, specularFactor * light.specular, light.ambient);
} 

vec3 calculatePointLight(PointLight light, vec3 normal) {
	vec3 lightDir = light.position - fs_in.fragPos;
	float diffuseFactor = getDiffuseFactor(lightDir, normal);
	float specularFactor = getSpecularFactor(lightDir, camPos - fs_in.fragPos, normal);
	
	float attenuation = getAttenuation(light.kc, light.kl, light.kq, length(lightDir));

	return calculateLightColor(diffuseFactor * light.diffuse, specularFactor * light.specular, light.ambient) * attenuation;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal) {
	vec3 lightDir = normalize(light.direction);
	vec3 fragDir = normalize(fs_in.fragPos - light.position);

	float angleBetween = max(dot(lightDir, fragDir), 0);
	float intensity = max((angleBetween - light.cutOffOuter)/(light.cutOffInner - light.cutOffOuter), 0.0);
	float attenuation = getAttenuation(light.kc, light.kl, light.kq, length(light.position - fs_in.fragPos));

	float diffuseFactor = getDiffuseFactor(-lightDir, normal);
	float specularFactor = getSpecularFactor(-lightDir, camPos - fs_in.fragPos, normal);

	return calculateLightColor(diffuseFactor * light.diffuse, specularFactor * light.specular, light.ambient) * attenuation * intensity;
}

void main() {
	fragColor = vec4(calculateSpotLight(spotLight, fs_in.normal), 1.0f);
}
