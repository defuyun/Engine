#include "tests.h"
#include "shader.h"
#include "stb_image.h"
#include "logger.h"
#include "camera.h"
#include "model.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>

namespace TMisc {
	void processInput(GLFWwindow *window);
	void mouseCallback(GLFWwindow * window, double xpos, double ypos);

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

	Camera * cam = nullptr;

	GLuint frameTexId = 0;

	void renderFrameTexture();

	struct Light {
		Light(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient) : diffuse(diffuse), specular(specular), ambient(ambient) {}

		glm::vec3 diffuse;
		float padding1;
		glm::vec3 specular;
		float padding2;
		glm::vec3 ambient;
		float padding3;
	};

	struct DirectionLight : public Light {
		DirectionLight(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient, const glm::vec3 & direction) : Light(diffuse, specular, ambient), direction(direction) {}
		glm::vec3 direction;
		float padding4;
	};

	GLfloat vertices[] = {
	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f, 0.0f
	};

	GLuint elements[] = {
		0, 1, 2, 2, 3, 0
	};
}

void testMisc() {
	using namespace TMisc;
	GLFWwindow * window = nullptr;
	
	const int WIN_WIDTH = 1280;
	const int WIN_HEIGHT = 720;

	// initialize sector
	{
		if (!glfwInit()) {
			Logger->log("[ENG] Can't init glfw\n");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "texture", NULL, NULL);

		if (!window) {
			Logger->log("failed to create window\n");
		}

		glfwMakeContextCurrent(window);

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouseCallback);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			Logger->log("[ENG] Can't initialize glew\n");
		}

		glEnable(GL_DEPTH_TEST);
	}

	Shader objectShader("shaders/object.vert", "shaders/object.frag");
	Shader shadowShader("shaders/shadow.vert", "shaders/shadow.frag");
	Shader textureShader("shaders/texture.vert", "shaders/texture.frag");
	Shader normalShader("shaders/normal.vert", "shaders/highlight.frag", "shaders/normal.geom");

	glm::vec3 camPos = glm::vec3(0.0f, 5.0f, -2.0f);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)(WIN_WIDTH) / WIN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view;

	cam = new Camera(camPos);

	glm::vec3 lightDir = glm::vec3(-1.0f, -1.0f, -1.0f);

	DirectionLight directionLight(
		glm::vec3(0.8f, 0.8f, 0.8f), // diffuse
		glm::vec3(0.2f, 0.2f, 0.5f), // specular
		glm::vec3(0.2f, 0.2f, 0.5f), // ambient
		lightDir
	);

	glm::vec3 lightPos = glm::vec3(0.0f) - 10.0f * lightDir;

	glm::vec3 right = glm::cross(lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 up = glm::cross(right, lightDir);

	glm::mat4 shadowView = glm::lookAt(lightPos, lightDir + lightPos, up);
	glm::mat4 shadowProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
	
	Model box("resources/box/box.vertices");
	Model plane("resources/box/plane.vertices");

	GLuint shadowFBO;
	glGenFramebuffers(1, &shadowFBO);
	
	const int SHADOW_WIDTH = 1280;
	const int SHADOW_HEIGHT = 1280;

	GLuint shadowTex;
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(perspective));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo, 0, 2 * sizeof(glm::mat4));

	GLuint lightUBO;
	glGenBuffers(1, &lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionLight), &directionLight, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 2, lightUBO, 0,sizeof(DirectionLight));

	objectShader.use();
	{
		objectShader.setVec3("lightPos", lightPos);
		glUniform1f(glGetUniformLocation(objectShader.ID, "material.shininess"), 16.0f);
	}

	shadowShader.use(); {
		shadowShader.setMat4("projection", shadowProj);
		shadowShader.setMat4("view", shadowView);
	}

	auto render = [&box, &plane](const Shader & shader) {
		shader.use();
		shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 5.0f)));
		box.draw(shader);

		shader.use();
		shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, .5f, 4.0f)));
		box.draw(shader);

		shader.use();
		shader.setMat4("model", glm::mat4(1.0f));
		plane.draw(shader);
	};

	lastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		currentFrame = (float)glfwGetTime();

		processInput(window);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		render(shadowShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
		glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getView()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		objectShader.use();
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, shadowTex);
		objectShader.setInt("depthMap", 5);
		objectShader.setMat4("lightMatrix", shadowProj * shadowView);
		render(objectShader);

		normalShader.use();
		render(normalShader);

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastFrame = currentFrame;
	}

	glfwTerminate();
}


void TMisc::renderFrameTexture() {
	if (frameTexId == 0) {
		glGenVertexArrays(1, &frameTexId);

		GLuint vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(frameTexId);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TMisc::vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TMisc::elements), elements, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	glBindVertexArray(frameTexId);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void TMisc::processInput(GLFWwindow *window)
{
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
}

void TMisc::mouseCallback(GLFWwindow * window, double xpos, double ypos) {
	cam->processMouseMovement(xpos, ypos, currentFrame - lastFrame);
}
