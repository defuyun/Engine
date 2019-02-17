#include "engine.h"
#include <iostream>

void Engine::createMatrixUBO() {
	glGenBuffers(1, &matrixUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matrixUBO);
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glm::mat4 projection = glm::perspective(45.0f, (float)width / height, 0.1f, 75.0f);
	glm::mat4 view = cam->getView();

	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &camPosUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, camPosUBO);

	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(cam->getPos()));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, MATRIX_BINDING, matrixUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERAPOS_BINDING, camPosUBO);
}

void Engine::updateView() {
	glBindBuffer(GL_UNIFORM_BUFFER, matrixUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getView()));

	glBindBuffer(GL_UNIFORM_BUFFER, camPosUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(cam->getPos()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Engine::init(int width, int height) {
	if (!glfwInit()) {
		std::cout << "[ENG] Can't init glfw\n";
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	this->window = glfwCreateWindow(width, height, "window", NULL, NULL);

	if (!window) {
		std::cout << "failed to create window\n";
	}

	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "[ENG] Can't initialize glew\n";
	}

}	

void Engine::pollEvent() {
	glfwPollEvents();
	processInput(window);
	processMousePos(window);
}

void Engine::processInput(GLFWwindow *window) {
	if (!cam)
		return;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float delta = currentFrame - lastFrame;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->processKeyPress(CameraDirection::front, delta);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->processKeyPress(CameraDirection::back, delta);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->processKeyPress(CameraDirection::left, delta);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->processKeyPress(CameraDirection::right, delta);

	if (keyPress['N'] && glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) {
		keyPress['N'] = false;
		displayNormal = !displayNormal;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		keyPress['N'] = true;
	}
}

void Engine::processMousePos(GLFWwindow * window) {
	if (!cam)
		return;

	GLdouble xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	cam->processMouseMovement(xPos, yPos, currentFrame - lastFrame);
}

void Engine::renderQuad() {
	if (quadVAO == 0) {
		GLfloat vertices[] = {
			-1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 1.0f
		};

		GLuint elements[] = {
			2, 3, 0, 0, 1 ,2
		};

		glGenVertexArrays(1, &quadVAO);

		GLuint vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(quadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
