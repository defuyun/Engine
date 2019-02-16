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

layout (std140, binding = 4) uniform LightBlock {
	DirectionLight directionLight;
	PointLight pointLight;
	SpotLight spotLight[2];
};

void main() {
	if (directionLight.type == 1 
		&& directionLight.ambient == vec3(0.2, 0.3,0.3) 
		&& directionLight.diffuse == vec3(0.2,0.3,0.3) 
		&& directionLight.direction == vec3(0.0f, -1.0f, -1.0f)
		&& pointLight.type == 2
		&& pointLight.ambient == vec3(0.4, 0.5, 0.5)
		&& pointLight.position == vec3(3.0f, 3.0f, 3.0f)
		&& pointLight.kc == 0.2f
		&& pointLight.kl == 0.3f
		&& pointLight.kq == 0.5f
		&& spotLight[0].ambient == vec3(0.2, 0.3, 0.3)
		&& spotLight[0].type == 3
		&& spotLight[1].ambient == vec3(0.4, 0.6, 0.7)
		&& spotLight[1].type == 3
	)
		fragColor = vec4(1.0f);
	else
		fragColor = vec4(0.0f);
}