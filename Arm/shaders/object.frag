#version 430 core

out vec4 fragColor;

in VS_OUT {
	vec3 normal;
	vec3 fragPos;
	vec2 texCoord;
	vec4 fragLightPos;
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
uniform sampler2D depthMap;

uniform vec3 lightPos;

float calcShadow() {
	vec2 sampleDirections[9] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, -1.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, -1.0f),
		vec2(-1.0f, 0.0f),
		vec2(-1.0f, 1.0f),
		vec2(-1.0f, 1.0f)
	};

	float bias = max(0.05 * (1.0f - dot(vs_in.normal, -directionLight.direction)), 0.005);
	float displace = length(lightPos - vs_in.fragPos);
	float totalDp = 0;

	for(int i = 0; i < 9; i++) {
		vec3 screenSpaceCoord = (0.004 / displace) * vec3(sampleDirections[i], 0.0f) + (vs_in.fragLightPos.xyz / vs_in.fragLightPos.w) * 0.5 + 0.5;
		float depth = texture(depthMap, screenSpaceCoord.xy).r;
		totalDp += screenSpaceCoord.z > depth + bias ? .0f : 1.0f;
	}

	return totalDp / 9;
}

vec3 calcDirectionLight(DirectionLight directionLight) {
	vec3 norm = normalize(vs_in.normal);
	vec3 lightDir = normalize(-directionLight.direction);
	float diffuseFactor = max(dot(lightDir, norm),0.0);

	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 eyeDir = normalize(camPos - vs_in.fragPos);
	float specularFactor = pow(max(dot(eyeDir, reflectDir),0), material.shininess);
	
	vec3 ambient = directionLight.ambient * texture(material.diffuse0, vs_in.texCoord).rgb;
	vec3 diffuse = directionLight.diffuse * diffuseFactor * texture(material.diffuse0, vs_in.texCoord).rgb;
	vec3 specular = directionLight.specular * specularFactor * texture(material.specular0, vs_in.texCoord).rgb;
	
	return ambient + calcShadow() * (diffuse + specular);
}

vec3 calcDL(DirectionLight directionLight) {
	vec3 norm = normalize(vs_in.normal);
	vec3 lightDir = normalize(-directionLight.direction);
	float diffuseFactor = max(dot(lightDir, norm),0.0);

	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 eyeDir = normalize(camPos - vs_in.fragPos);
	float specularFactor = pow(max(dot(eyeDir, reflectDir),0), material.shininess);
		
	return normalize(vec3(diffuseFactor, specularFactor, calcShadow()));
}

void main() {
	fragColor = vec4(calcDirectionLight(directionLight), 1.0f);
	// fragColor = vec4(calcDL(directionLight), 1.0f);
	// fragColor = vec4(directionLight.direction, 1.0f);
}