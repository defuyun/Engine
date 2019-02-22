#include "engine.h"
#include "model.h"
#include <iostream>


void Engine::bindShadowMap(const Shader & shader) const {
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
	shader.use();
	shader.setInt(shadowMapLocation, GLint(5));
}

void Engine::renderToFrame(GLuint fbo, const std::function<void()> & render) const {
	render();
}

void Engine::beginRender(GLuint fbo) const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Engine::endRender() const {
	glActiveTexture(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::renderFrameToScreen(GLuint textureId, const glm::mat4 & model, const Shader & quadShader) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureId);
	quadShader.use();
	quadShader.setInt("tex", (GLint)1);
	quadShader.setBool("gamma", Model::useGamma);
	quadShader.setMat4("model", model);
	renderQuad();
}

void Engine::createShadowMap(const glm::vec3 & lightPos, const Shader & shadowShader, const std::function<void(const Shader &)> & render) {
	if (shadowMapFBO == 0) {
		initShadowMap(shadowShader);
	}

	glm::vec3 shadowDirections[6] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
	};

	glm::vec3 shadowUpDirections[6] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
	};

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shadowShader.use();
	shadowShader.setVec3("lightPos", lightPos);

	for (int i = 0; i < 6; ++i)
		shadowShader.setMat4("shadowView[" + std::to_string(i) + ']', glm::lookAt(lightPos, lightPos + shadowDirections[i], shadowUpDirections[i]));

	renderingShadow = true;
	render(shadowShader);
	renderingShadow = false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

void Engine::initShadowMap(const Shader & shadowShader) {
	glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, SHADOW_NEAR, SHADOW_FAR);

	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);

	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	 shadowShader.use();
	 shadowShader.setMat4("shadowProjection", shadowProjection);
	 shadowShader.setFloat("near", SHADOW_NEAR);
	 shadowShader.setFloat("far", SHADOW_FAR);
}

void Engine::createFBO(int width, int height, GLuint * fbo, GLuint * texture) {
	GLuint depthStencil;

	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);

	glGenRenderbuffers(1, &depthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, depthStencil);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencil);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "framebuffer incomplete\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::createSceneFBO() {
	if (sceneFBO != 0) {
		glDeleteFramebuffers(1, &sceneFBO);
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	createFBO(width, height, &sceneFBO, &sceneTexture);
}

void Engine::createShadowSkyboxFBO() {
	if (shadowSkyboxFBO != 0) {
		glDeleteFramebuffers(1, &shadowSkyboxFBO);
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	createFBO(width, height, &shadowSkyboxFBO, &shadowSkyboxTexture);
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
