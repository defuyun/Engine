#pragma once

#include <gl/glew.h>

#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

#include <vector>

const int DIRECTIONAL = 1;
const int POINT = 2;
const int SPOT = 3;

const int LIGHT_BINDING = 4;

// aligned to std140

struct Light {
	glm::vec3 ambient;
	float offsetAmbient; // 16

	glm::vec3 diffuse;
	float offsetDiffuse; // 32

	glm::vec3 specular;
	int type; // 48

	Light(int type, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular) :
		type(type), ambient(ambient), diffuse(diffuse), specular(specular) {}
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
	float kc; // 64

	float kl; // 68
	float kq; // 72

	float offsetK[2]; // 80

	PointLight(int type, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular,
		const glm::vec3 & position, float kc, float kl, float kq) :
		Light(type, ambient, diffuse, specular),position(position), kc(kc), kl(kl), kq(kq) {}
};

struct SpotLight : public PointLight {
	glm::vec3 direction;
	float cutOffInner; // 96

	float cutOffOuter; // 100
	glm::vec3 offsetCutOfff; // 112

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

	template<typename T>
	void updateLightParameter(int type, int index, int offset, T data) {
		int position = 0;
		int structSize = 0;
		switch (type)
		{
		case DIRECTIONAL:
			structSize = sizeof(DirectionLight);
			break;
		case POINT:
			position = directionLightCount * sizeof(DirectionLight);
			structSize = sizeof(PointLight);
			break;
		case SPOT:
			position = directionLightCount * sizeof(DirectionLight) + pointLightCount * sizeof(PointLight);
			structSize = sizeof(SpotLight);
			break;
		default:
			break;
		}

		position += index * structSize + offset;

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, position, sizeof(T), &data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
};

