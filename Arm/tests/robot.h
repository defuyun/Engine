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

	std::shared_ptr<Object> wire;

	bool active[TOTAL_ARMS] = { false };
	enum Operation {MOVE_ARM, ROTATE_CLUTCH, MOVE_CLUTCH, TOGGLE};
	bool sanityCheck(Operation op, int id, float value);
	
	bool wireClutched[TOTAL_ARMS] = { false };
	std::shared_ptr<Object> constructHierarchy(const std::shared_ptr<Object> & root);
public:
	Robot(const std::shared_ptr<Object> & object);

	void moveArm(float displacement);
	void rotateClutch(float angle, float speed);
	void moveClutch(float displacement);
	void toggleArm(int id);
	void draw(const Shader & objshader, const Shader & wireShader) const;
};

void run();