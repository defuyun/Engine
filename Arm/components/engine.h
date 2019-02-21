#pragma once

#include "camera.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>

const int MATRIX_BINDING = 1;
const int CAMERAPOS_BINDING = 5;

class Engine {
public:
	void renderQuad();
	void renderCube();

	Camera * cam = nullptr;
	GLFWwindow * window = nullptr;

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

	GLuint sceneFBO = 0;
	GLuint sceneTexture = 0;
	GLuint scenceDepthStencil = 0;

	void init(int width, int height);
	void pollEvent();

	void createMatrixUBO();
	void updateView();
	void createSceneFBO();

	bool displayNormal = false;
	bool useBlinn = true;
private:
	GLuint quadVAO = 0;
	GLuint cubeVAO = 0;
	GLuint matrixUBO = 0;
	GLuint camPosUBO = 0;

	bool keyPress[256] = { false };

	void processInput(GLFWwindow *window);
	void processMousePos(GLFWwindow * window);
};
