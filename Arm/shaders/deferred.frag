#version 430

out vec4 fragColor;

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 position;
	vec3 color;
};

in VS_OUT {
	vec2 texCoord;
} fs_in;

layout (std140, binding = 5) uniform CamPosBlack {
	vec3 camPos;
};

const int lightCount = 5;

layout (std140, binding = 4) uniform LightBlock {
	Light lights[lightCount];
};


vec3 lightCalc(vec3 fragPos, vec3 camPos, vec3 normal, vec3 diffuse, float specular, Light light) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 camDir = normalize(camPos - fragPos);

	float d = max(0, dot(lightDir, normalize(normal)));
	vec3 H = normalize(lightDir + camDir);

	float s = pow(max(0, dot(H, normal)), 32.0f);

	return d * diffuse * light.color + vec3(s * specular) * light.color + vec3(0.1f);
}

void main() {
	vec3 fragPos = texture(gPos, fs_in.texCoord).rgb; 
	vec3 normal = texture(gNormal, fs_in.texCoord).rgb;
	vec4 albdedoSpec = texture(gAlbedoSpec, fs_in.texCoord);

	vec3 diffuse = albdedoSpec.rgb;
	float specular = albdedoSpec.a;

	vec3 color = vec3(0.0f);
	for (int i = 0; i < lightCount; i++) {
		color += lightCalc(fragPos, camPos, normal, diffuse, specular, lights[i]);
	}

	fragColor = vec4(color, 1.0f);
}