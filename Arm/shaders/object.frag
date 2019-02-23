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
	sampler2D normal0;
	sampler2D normal1;
	float shininess;
};

layout (std140, binding = 4) uniform LightBlock {
	DirectionLight directionLight;
	PointLight pointLight;
	SpotLight spotLight;
};


in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 camPos;
	vec2 texCoord;

	vec3 fragTangentPos;
	vec3 camTangentPos;
	vec3 lightTangentPos;

	mat3 tbn;
} fs_in;

uniform Material material;
uniform bool blinn;
uniform bool useNormalMap;

uniform samplerCube shadowMap;
uniform float lightNear;
uniform float lightFar;

float shadowSamples = 20;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float calculateLightShadow(vec3 lightPos, vec3 fragPos, vec3 normal) {
	vec3 lightDir = normalize(fragPos - lightPos);
	float diskRadius = 0.005f;

	float bias = max(0.005 * (1 - dot(-lightDir, normal)), 0.0005);
	float shadows = 0.0f;

	for (int i = 0; i < 20; ++i) {
		float currentDepth = length(fragPos - lightPos) / (lightFar - lightNear);
		float depth = texture(shadowMap, lightDir + diskRadius * sampleOffsetDirections[i]).r + bias;
		shadows  += depth > currentDepth ? 0.0f : 1.0f;
	}

	return shadows / shadowSamples;
}

// light direction in reverse incident direction : from fragPos to lightPos
float getDiffuseFactor(vec3 lightDirection, vec3 normal) {
	return max(dot(normalize(lightDirection), normalize(normal)), 0);
}

// light direction in reverse incident direction
// cam direction from fragment to camera position
float getSpecularFactor(vec3 lightDirection, vec3 camDirection, vec3 normal) {
	float spec = 0;

	if (blinn) {
		vec3 halfway = normalize(normalize(lightDirection) + normalize(camDirection));
		spec = max(dot(normalize(normal), halfway), 0);
	} else {
		vec3 reflectedLight = normalize(reflect(normalize(-lightDirection), normalize(normal)));
		spec = max(dot(normalize(camDirection), reflectedLight), 0); 
	}

	return pow(spec, material.shininess);
}

float getAttenuation(float kc, float kl, float kq, float dist) {
	return 1.0 / (kc + kl * dist + kq * dist);
}

vec3 calculateLightColor(vec3 diffuse, vec3 specular, vec3 ambient, vec3 lightPos, vec2 texCoord) {
	float shadow = calculateLightShadow(lightPos, fs_in.fragPos, fs_in.normal);

	vec3 diffuseColor = diffuse * texture(material.diffuse0, texCoord).rgb; 
	vec3 specularColor = specular * texture(material.specular0, texCoord).rgb;
	vec3 ambientColor = ambient * texture(material.diffuse0, texCoord).rgb; 

	return  (1.0 - shadow) * (diffuseColor + specularColor) + ambientColor;
}

vec3 calculateDirectionLight(DirectionLight light, vec3 normal) {
	float diffuseFactor = getDiffuseFactor(-light.direction, normal);
	float specularFactor = getSpecularFactor(-light.direction, fs_in.camPos - fs_in.fragPos, normal);

	vec3 estimatePos = vec3(0.0f, 0.0f, 0.0f) - 5.0 * normalize(light.direction);
	return calculateLightColor(diffuseFactor * light.diffuse, specularFactor * light.specular, light.ambient, estimatePos, fs_in.texCoord);
} 

vec3 calculateSpotLight(SpotLight light, vec3 normal) {
	vec3 lightDir = normalize(light.direction);
	vec3 fragDir = normalize(fs_in.fragPos - light.position);

	float angleBetween = max(dot(lightDir, fragDir), 0);
	float intensity = max((angleBetween - light.cutOffOuter)/(light.cutOffInner - light.cutOffOuter), 0.0);
	float attenuation = getAttenuation(light.kc, light.kl, light.kq, length(light.position - fs_in.fragPos));

	float diffuseFactor = getDiffuseFactor(-lightDir, normal);
	float specularFactor = getSpecularFactor(-lightDir, fs_in.camPos - fs_in.fragPos, normal);

	return calculateLightColor(diffuseFactor * light.diffuse, specularFactor * light.specular, light.ambient, light.position, fs_in.texCoord) * attenuation * intensity;
}

vec3 calculatePointLight(PointLight light, vec3 lightPos, vec3 fragPos, vec3 camPos, vec3 normal) {
	vec3 lightDir = lightPos - fragPos; 
	float diffuseFactor = getDiffuseFactor(lightDir, normal);
	float specularFactor = getSpecularFactor(lightDir, camPos - fragPos, normal);
	
	float attenuation = getAttenuation(light.kc, light.kl, light.kq, length(light.position - fs_in.fragPos));
	float shadow = calculateLightShadow(lightPos, fs_in.fragPos, fs_in.normal);

	vec2 viewDir = normalize(fragPos - camPos).xy;
	float height = texture(material.normal1, fs_in.texCoord).r;

	vec2 newCoord = fs_in.texCoord - height * viewDir * 0.001;

	return calculateLightColor(diffuseFactor * light.diffuse, specularFactor * light.specular, light.ambient, light.position, newCoord) * attenuation;
}

void main() {
	if (useNormalMap) { 
		vec3 normal = normalize((texture(material.normal0, fs_in.texCoord).rgb * 2) - 1);
		fragColor = vec4(calculatePointLight(pointLight, fs_in.lightTangentPos, fs_in.fragTangentPos, fs_in.camTangentPos, normal), 1.0f);
	} else {
		fragColor = vec4(calculatePointLight(pointLight, pointLight.position, fs_in.fragPos, fs_in.camPos, fs_in.normal), 1.0f);
	}
}
