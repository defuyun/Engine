#pragma once

#include <gl/glew.h>

#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

#include <vector>

const int DIRECTIONAL = 1;
const int POINT = 2;
const int SPOT = 3;

// aligned to std140

struct Light {
	int type;
	glm::vec3 ambient;
	float offsetAmbient;
	glm::vec3 diffuse;
	float offsetDiffuse;
	glm::vec3 specular;
	float offsetSpecular;
	Light(int type, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular) :
		ambient(ambient), diffuse(diffuse), specular(specular) {}
};

struct DirectionLight : public Light {
	glm::vec3 direction;
	float offsetDirection;
	DirectionLight(int type, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular,
		const glm::vec3 & direction) :
		Light(type, ambient, diffuse, specular), direction(direction) {}
};

struct PointLight : public Light {
	glm::vec3 position;
	float offsetPosition;
	float kc;
	float kl;
	float kq;
	PointLight(int type, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular,
		const glm::vec3 & position, float kc, float kl, float kq) :
		Light(type, ambient, diffuse, specular),position(position), kc(kc), kl(kl), kq(kq) {}
};

struct SpotLight : public PointLight {
	glm::vec3 direction;
	float offsetDirection;
	float cutOffInner;
	float cutOffOuter;

	SpotLight(int type, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular,
		const glm::vec3 & position, float kc, float kl, float kq, const glm::vec3 & direction, float cutOffInner, float cutOffOuter) :
		PointLight(type, ambient, diffuse, specular, position, kc, kl, kq), direction(direction), cutOffInner(cutOffInner), cutOffOuter(cutOffOuter) {}
};

class LightEngine {
public:
	int directionLightCount = 0;
	int pointLightCount = 0;
	int spotLightCount = 0;
		
	GLuint ubo = 0;

	GLuint createLightUBO(const std::vector<Light *>  & lights);
};

