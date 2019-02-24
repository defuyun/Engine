#pragma once

#include <gl/glew.h>

#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

#include <vector>

const int LIGHT_BINDING = 4;

struct SimpleLight {
	glm::vec3 position;
	float offsetPositin;
	glm::vec3 color;
	float offsetColor;

	SimpleLight(const glm::vec3 & position, const glm::vec3 & color) : position(position), color(color) {}
};

class LightEngine {
public:
	GLuint ubo = 0;
	std::vector<SimpleLight> lights;

	void updateSimpleLight() const;

	LightEngine(const std::vector<SimpleLight> & lights) : lights(lights) {
		createSimpleLightUBO(lights);
	}
private:
	void createSimpleLightUBO(const std::vector<SimpleLight> & lights);
};

