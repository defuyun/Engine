#include "engine.h"
#include "model.h"
#include <iostream>

void Engine::createSceneFBO() {
	if (sceneFBO != 0) {
		glDeleteFramebuffers(1, &sceneFBO);
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glGenFramebuffers(1, &sceneFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

	glGenTextures(1, &sceneTexture);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);

	glGenRenderbuffers(1, &scenceDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, scenceDepthStencil);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, scenceDepthStencil);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "framebuffer incomplete\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::createMatrixUBO() {
	if (matrixUBO != 0) {
		glDeleteBuffers(1, &matrixUBO);
	}

	glGenBuffers(1, &matrixUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matrixUBO);
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / height, 0.1f, 75.0f);
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

	if (keyPress['B'] && glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		keyPress['B'] = false;
		useBlinn = !useBlinn;
	}

	if (keyPress['G'] && glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
		keyPress['G'] = false;
		Model::useGamma = !Model::useGamma;
	}

	if (keyPress['Q'] && glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
		keyPress['Q'] = false;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		keyPress['N'] = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		keyPress['B'] = true;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		keyPress['G'] = true;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		keyPress['Q'] = true;
	}
}

void Engine::processMousePos(GLFWwindow * window) {
	if (!cam)
		return;

	GLdouble xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	cam->processMouseMovement(xPos, yPos, currentFrame - lastFrame);
}


void Engine::renderCube() {
	if (cubeVAO == 0) {
		float vertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		glGenVertexArrays(1, &cubeVAO);

		GLuint vbo;
		glGenBuffers(1, &vbo);

		glBindVertexArray(cubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);;
	glBindVertexArray(0);
}

void Engine::renderQuad() {
	if (quadVAO == 0) {
		GLfloat vertices[] = {
			-1.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f
		};

		GLuint elements[] = {
			2, 0, 3, 0, 2 ,1
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
