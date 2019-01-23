#pragma once

#include <glm\gtc\type_ptr.hpp>
#include "scene.h"
#include "shader.h"

constexpr short TOTAL_ARMS = 4;

struct Clutch {
	std::shared_ptr<Object> left = nullptr;
	std::shared_ptr<Object> right = nullptr;
};

class Robot {
private:
	std::shared_ptr<Object> root;
	std::shared_ptr<Object> arms[TOTAL_ARMS];
	std::shared_ptr<Object> rotateUnit[TOTAL_ARMS];
	Clutch clutchUnit[TOTAL_ARMS];

	bool active[TOTAL_ARMS] = { false };
public:
	Robot(const std::shared_ptr<Object> & object);
	std::shared_ptr<Object> constructHierarchy(const std::shared_ptr<Object> & root);

	void moveArm(float displacement);
	void rotateClutch(float angle);
	void moveClutch(float displacement);
	void toggleArm(int id);
	void draw(const Shader & shader) const;
};

void run();