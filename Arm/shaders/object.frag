#version 430

out vec4 fragColor;

struct DirectionLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int type;
	vec3 direction;
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
};

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 camPos;
	vec2 texCoord;

	vec3 fragTangentPos;
	vec3 camTangentPos;
	vec3 lightTangentDir;

	mat3 tbn;
} fs_in;

uniform Material material;
uniform bool useBlinn;
uniform bool useNormalMap;
uniform bool useDepthMap;

uniform float heightScale;

// light direction in reverse incident direction : from fragPos to lightPos
float getDiffuseFactor(vec3 lightDirection, vec3 normal) {
	return max(dot(normalize(lightDirection), normalize(normal)), 0);
}

// light direction in reverse incident direction
// cam direction from fragment to camera position
float getSpecularFactor(vec3 lightDirection, vec3 camDirection, vec3 normal) {
	vec3 halfway = normalize(normalize(lightDirection) + normalize(camDirection));
	float spec = max(dot(normalize(normal), halfway), 0);
	return pow(spec, material.shininess);
}

vec3 getColor(sampler2D textureId, vec2 texCoord) {
	return texture(textureId, texCoord).rgb;
}

vec2 getParallexTexCoord(sampler2D heightMap, vec2 texCoord, vec3 camDirection) {
	float numLayers = 32;
	float layerDepth = 1 / numLayers;

	vec3 ncam = normalize(camDirection);
	vec2 tangent = ncam.xy / ncam.z * heightScale;

	vec2 perLayerTangent = tangent / numLayers;
	
	float currentLayerDepth = 0.0f;
	vec2 currentTexCoord = texCoord;

	float currentHeightMap = texture(heightMap, currentTexCoord).r;

	while(currentLayerDepth < currentHeightMap) {
		currentLayerDepth += layerDepth;
		currentTexCoord -= perLayerTangent;
		currentHeightMap = texture(heightMap, currentTexCoord).r;
	}

	return currentTexCoord;

	vec2 prevCoord = currentTexCoord + perLayerTangent;
	float prevHeightMap = texture(heightMap, prevCoord).r;
	float prevLayerDepth = currentLayerDepth - layerDepth;
	
	float d1 = currentLayerDepth - currentHeightMap;
	float d2 = prevHeightMap - prevLayerDepth;

	float weight = d2 / (d1 + d2);

	return weight * prevCoord + (1 - weight) * currentTexCoord;
}

void main() {
	
	vec3 normal, lightDirection, camDirection;
	vec2 texCoord;

	if (useNormalMap) {
		lightDirection = fs_in.lightTangentDir;
		camDirection = fs_in.camTangentPos - fs_in.fragTangentPos;
	} else {
		lightDirection = -directionLight.direction;
		camDirection = fs_in.camPos - fs_in.fragPos;
	}

	if (useDepthMap) {
		texCoord = getParallexTexCoord(material.normal1, fs_in.texCoord, camDirection);
	} else {
		texCoord = fs_in.texCoord;
	}

	if (useNormalMap) {
		normal = normalize((texture(material.normal0, texCoord).rgb * 2) - 1);
	} else {
		normal = fs_in.normal;
	}

	float diffuse = getDiffuseFactor(lightDirection, normal);
	float specular = getSpecularFactor(lightDirection, camDirection, normal);

	vec3 d = directionLight.diffuse * diffuse * getColor(material.diffuse0, texCoord);
	vec3 s = directionLight.specular * specular * getColor(material.specular0, texCoord);
	vec3 a = directionLight.ambient * getColor(material.diffuse0, texCoord);

	fragColor = vec4(d + s + a, 1.0f);
}
