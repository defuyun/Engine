#include "tests.h"

#include "tests.h"
#include "shader.h"
#include "stb_image.h"
#include "logger.h"
#include "camera.h"

#include <GL/glew.h>
#include<GLFW\glfw3.h>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

namespace TLighting {
	void processInput(GLFWwindow *window);
	void mouseCallback(GLFWwindow * window, double xpos, double ypos);

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

	Camera * cam = nullptr;
}


void testLighting() {
	using namespace TLighting;
	if (!glfwInit()) {
		Logger->log("[ENG] Can't init glfw\n");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow * window = glfwCreateWindow(800, 640, "texture", NULL, NULL);

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

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	GLuint vao, vbo;

	Shader objectShader("tests/objectShader.vert", "tests/objectShader.frag");
	Shader lightShader("tests/lightShader.vert", "tests/lightShader.frag");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint posId = glGetAttribLocation(objectShader.ID, "aPos");
	GLuint normalId = glGetAttribLocation(objectShader.ID, "aNormal");
	GLuint texCoordId = glGetAttribLocation(objectShader.ID, "aTexCoord");

	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(posId);
	
	glVertexAttribPointer(normalId, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(normalId);

	glVertexAttribPointer(texCoordId, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(texCoordId);

	GLuint lightVao;
	glGenVertexArrays(1, &lightVao);
	glBindVertexArray(lightVao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	posId = glGetAttribLocation(lightShader.ID, "aPos");
	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(posId);

	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	GLuint diffuseMap;
	{
		glGenTextures(1, &diffuseMap);

		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
		int width, height, nrChannel;
		auto data = stbi_load("images/rock.jpg", &width, &height, &nrChannel, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		stbi_image_free(data);
	}


	glActiveTexture(GL_TEXTURE1);
	GLuint specularMap;
	{
		glGenTextures(1, &specularMap);

		glBindTexture(GL_TEXTURE_2D, specularMap);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, nrChannel;
		auto data = stbi_load("images/rock_spec.jpg", &width, &height, &nrChannel, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
	}

	glm::vec3 camPos = glm::vec3(0.0f, 1.0f, -1.0f);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)(800) / 640, 0.1f, 10.f);
	glm::mat4 view;

	cam = new Camera(camPos);

	objectShader.use();
	glm::mat4 modelBox = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f));

	glm::vec3 lightDiffuse = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 lightSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
	glm::vec3 lightAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	
	glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelBox));
	glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));

	glUniform1i(glGetUniformLocation(objectShader.ID, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(objectShader.ID, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(objectShader.ID, "material.shininess"), 16.0f);

	glUniform3fv(glGetUniformLocation(objectShader.ID, "light.diffuse"), 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(glGetUniformLocation(objectShader.ID, "light.specular"), 1, glm::value_ptr(lightSpecular));
	glUniform3fv(glGetUniformLocation(objectShader.ID, "light.ambient"), 1, glm::value_ptr(lightAmbient));

	lightShader.use();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));
	
	lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();

		processInput(window);
		view = cam->getView();
		glm::vec3 lightPos = glm::vec3(glm::cos(currentFrame), glm::sin(currentFrame),2.0f + glm::cos(currentFrame) * glm::sin(currentFrame));
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		objectShader.use();
		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "light.position"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "camPos"), 1, glm::value_ptr(cam->getPos()));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(lightVao);
		lightShader.use();
		glm::mat4 modelLight = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.2f, 0.2f, 0.2f)), currentFrame, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelLight));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastFrame = currentFrame;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &lightVao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();
}

void TLighting::processInput(GLFWwindow *window)
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

void TLighting::mouseCallback(GLFWwindow * window, double xpos, double ypos) {
	cam->processMouseMovement(xpos, ypos, currentFrame - lastFrame);
}