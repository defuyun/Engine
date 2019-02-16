#pragma once

#include "camera.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>

class Engine {
public:
	void renderQuad();

	Camera * cam = nullptr;
	GLFWwindow * window = nullptr;

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

	void init(int width, int height);
	void pollEvent();
private:
	GLuint quadVAO = 0;
	void processInput(GLFWwindow *window);
	void processMousePos(GLFWwindow * window);
};
