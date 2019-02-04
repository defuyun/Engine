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
}


void testMisc() {
	using namespace TMisc;
	GLFWwindow * window = nullptr;

	// initialize sector
	{
		if (!glfwInit()) {
			Logger->log("[ENG] Can't init glfw\n");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(800, 640, "texture", NULL, NULL);

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
	Shader normalShader("shaders/normal.vert", "shaders/outline.frag", "shaders/normal.geom");
	Shader highLightShader("shaders/highlight.vert", "shaders/highlight.frag", "shaders/highlight.geom");

	glm::vec3 camPos = glm::vec3(0.0f, 10.0f, -2.0f);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)(800) / 640, 0.1f, 100.0f);
	glm::mat4 view;

	cam = new Camera(camPos);

	DirectionLight directionLight(
		glm::vec3(0.3f, 0.3f, 0.5f), // diffuse
		glm::vec3(0.2f, 0.2f, 0.5f), // specular
		glm::vec3(0.2f, 0.2f, 0.5f), // ambient
		glm::vec3(0.0f, -1.0f, -1.0f) // direction
	);
	
	Model nanosuit("resources/nanosuit/nanosuit.obj");

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
	glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec3), &directionLight, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 2, lightUBO, 0, 4 * sizeof(glm::vec3));

	objectShader.use();
	{
		glUniform1f(glGetUniformLocation(objectShader.ID, "material.shininess"), 16.0f);
	}

	const int xMax = 10, yMax = 10;

	std::vector<glm::mat4> translation(xMax * yMax);
	
	float start_x = -(5.0 * xMax) * 0.5,  start_z = 3.0f;

	for (int i = 0; i < xMax; i++) {
		for (int j = 0; j < yMax; j++) {
			float new_x = start_x + 5.0 * i;
			float new_z = start_z + 5.0 * j;
			translation[i * 10 + j] = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(new_x, 0.0f, new_z)), glm::vec3(0.5f, 0.5f, 0.5f));
		}
	}

	GLuint instanceOffSetBuffer;
	glGenBuffers(1, &instanceOffSetBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, instanceOffSetBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * translation.size(), &translation[0], GL_STATIC_DRAW);
	
	nanosuit.instanceTranslation(instanceOffSetBuffer);

	lastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		currentFrame = (float)glfwGetTime();

		processInput(window);
		view = cam->getView();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
			
		nanosuit.drawInstance(objectShader, xMax * yMax);

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastFrame = currentFrame;
	}

	glfwTerminate();
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
