#include "light.h"
#include <iostream>

void LightEngine::createSimpleLightUBO(const std::vector<SimpleLight> & lights) {
	if (ubo != 0) {
		glDeleteBuffers(1, &ubo);
	}

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, lights.size() * sizeof(SimpleLight), &lights[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING, ubo);
}

void LightEngine::updateSimpleLight() const {
	if (ubo == 0) {
		std::cout << "light ubo has not been initialized\n";
		return;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, lights.size() * sizeof(SimpleLight), &lights[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
