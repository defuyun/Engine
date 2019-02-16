#include "light.h"
#include <iostream>

GLuint LightEngine::createLightUBO(const std::vector<Light *> & lights) {
	if (ubo != 0) {
		glDeleteBuffers(1, &ubo);
	}

	int totalSize = 0;
	
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	
	std::vector<DirectionLight> directionLightsCpy;
	std::vector<PointLight> pointLightsCpy;
	std::vector<SpotLight> spotLightsCpy;

	for (auto light : lights) {
		switch (light->type) {
		case DIRECTIONAL:
			directionLightsCpy.push_back(*((DirectionLight *)light));
			break;
		case POINT:
			pointLightsCpy.push_back(*((PointLight *)light));
			break;
		case SPOT:
			spotLightsCpy.push_back(*((SpotLight *)light));
			break;
		}
	}
	
	directionLightCount = directionLightsCpy.size();
	pointLightCount = pointLightsCpy.size();
	spotLightCount = spotLightsCpy.size();

	totalSize = directionLightCount * sizeof(DirectionLight) + pointLightCount * sizeof(PointLight) + spotLightCount * sizeof(SpotLight);

	if (totalSize > 16000) {
		std::cout << "[LIGHT] lighting information exceeded 16kb, there might be implication of resulting from this\n";
	}

	glBufferData(GL_UNIFORM_BUFFER, totalSize, NULL, GL_STATIC_DRAW);

	int dLightIndex = 0;
	int pLightIndex = directionLightCount * sizeof(DirectionLight);
	int sLightIndex = pLightIndex + pointLightCount * sizeof(PointLight);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, directionLightCount * sizeof(DirectionLight), &directionLightsCpy[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, pLightIndex, pointLightCount * sizeof(PointLight), &pointLightsCpy[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sLightIndex, spotLightCount * sizeof(SpotLight), &spotLightsCpy[0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING, ubo);
	return ubo;
}