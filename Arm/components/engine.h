#pragma once

#include "camera.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>

const int MATRIX_BINDING = 1;
const int CAMERAPOS_BINDING = 5;

class Engine {
public:
	void renderQuad();

	Camera * cam = nullptr;
	GLFWwindow * window = nullptr;

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

	void init(int width, int height);
	void pollEvent();

	void createMatrixUBO();
	void updateView();

	bool displayNormal = false;
private:
	GLuint quadVAO = 0;
	GLuint matrixUBO = 0;
	GLuint camPosUBO = 0;

	bool keyPress[256] = { false };

	void processInput(GLFWwindow *window);
	void processMousePos(GLFWwindow * window);
};
