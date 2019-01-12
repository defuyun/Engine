#include "tests.h"

#include "tests.h"
#include "shader.h"
#include "stb_image.h"
#include "logger.h"
#include "camera.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>

namespace TLighting {
	void processInput(GLFWwindow *window);
	void mouseCallback(GLFWwindow * window, double xpos, double ypos);

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

	Camera * cam = nullptr;
}

struct Light {
	Light(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient) : diffuse(diffuse), specular(specular), ambient(ambient) {}

	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
};

struct PointLight : public Light{
	PointLight(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient, float constant, float linear, float quadratic, const glm::vec3 & position) :
		Light(diffuse, specular, ambient), constant(constant), linear(linear), quadratic(quadratic) {}

	float constant;
	float linear;
	float quadratic;
	glm::vec3 position;
};

struct DirectionLight : public Light{
	DirectionLight(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient, const glm::vec3 & direction) : Light(diffuse, specular, ambient), direction(direction) {}
	glm::vec3 direction;
};

struct SpotLight : public PointLight{
	SpotLight(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient, 
		float constant, float linear, float quadratic, 
		const glm::vec3 & position, const glm::vec3 & direction, 
		float innerCutOff, float outerCutOff) :
		PointLight(diffuse, specular, ambient, constant, linear, quadratic, position), direction(direction), innerCutOff(innerCutOff), outerCutOff(outerCutOff) {}
	glm::vec3 direction;
	float innerCutOff;
	float outerCutOff;
};

struct Object {
	Object(const glm::vec3 & position, const glm::vec3 & scale = glm::vec3(1.0f), const glm::vec3 & rotateAxis = glm::vec3(1.0f), float rotateAngle = 0.0f) : position(position), scale(scale), rotateAxis(rotateAxis), rotateAngle(rotateAngle) {}
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotateAxis;
	float rotateAngle;
};

void testLighting() {
	using namespace TLighting;
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

	// vertices
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
	GLuint vao, vbo, lightVao;

	Shader objectShader("tests/objectShader.vert", "tests/objectShader.frag");
	Shader lightShader("tests/lightShader.vert", "tests/lightShader.frag");
	// binding and interpreting vertices
	{
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

		glGenVertexArrays(1, &lightVao);
		glBindVertexArray(lightVao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		posId = glGetAttribLocation(lightShader.ID, "aPos");
		glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
		glEnableVertexAttribArray(posId);

		glBindVertexArray(0);
	}

	// load diffuse map
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

	// load specular map
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
		auto data = stbi_load("images/rock.jpg", &width, &height, &nrChannel, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
	}

	glm::vec3 camPos = glm::vec3(0.0f, 1.0f, -1.0f);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)(800) / 640, 0.1f, 100.f);
	glm::mat4 view;

	cam = new Camera(camPos);

	std::vector<Object> objects = {
		Object(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(1.0f), glm::vec3(1.0f,0.0f, 0.0f), 30.0f),
		Object(glm::vec3(0.0f, 5.0f, 4.0f), glm::vec3(1.0f), glm::vec3(0.0f,1.0f, 0.0f), 45.0f),
		Object(glm::vec3(3.0f, -2.0f, 5.0f), glm::vec3(1.0f), glm::vec3(0.0f,0.0f, 1.0f), 60.0f),
		Object(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f), glm::vec3(1.0f,1.0f, 1.0f), 120.0f),
	};

	std::vector<PointLight> pointLights = {
		PointLight(glm::vec3(0.2f,0.2f, 0.8f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.2f, 0.2f, 0.2f), 1.0f, 0.3f, 0.8f, glm::vec3()),
		PointLight(glm::vec3(0.2f,0.2f, 0.8f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.2f, 0.2f, 0.2f), 1.0f, 0.3f, 0.8f, glm::vec3()),
		PointLight(glm::vec3(0.2f,0.2f, 0.8f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.2f, 0.2f, 0.2f), 1.0f, 0.3f, 0.8f, glm::vec3()),
		PointLight(glm::vec3(0.2f,0.2f, 0.8f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.2f, 0.2f, 0.2f), 1.0f, 0.3f, 0.8f, glm::vec3()),
	};

	DirectionLight directionLight(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, -1.0f, 0.0f));
	SpotLight spotLight(glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.7f, 0.6f, 0.4f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.1f, 0.1f, glm::vec3(), glm::vec3(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(30.0f)));

	objectShader.use();
	glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));
	glUniform1i(glGetUniformLocation(objectShader.ID, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(objectShader.ID, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(objectShader.ID, "material.shininess"), 16.0f);
	
	glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.diffuse"), 1, glm::value_ptr(directionLight.diffuse));
	glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.specular"), 1, glm::value_ptr(directionLight.specular));
	glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.ambient"), 1, glm::value_ptr(directionLight.ambient));
	glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.direction"), 1, glm::value_ptr(directionLight.direction));
	
	glUniform3fv(glGetUniformLocation(objectShader.ID, "spotLight.diffuse"), 1, glm::value_ptr(spotLight.diffuse));
	glUniform3fv(glGetUniformLocation(objectShader.ID, "spotLight.specular"), 1, glm::value_ptr(spotLight.specular));
	glUniform3fv(glGetUniformLocation(objectShader.ID, "spotLight.ambient"), 1, glm::value_ptr(spotLight.ambient));
	glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.innerCutOff"), spotLight.innerCutOff);
	glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.outerCutOff"), spotLight.outerCutOff);
	glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.constant"), spotLight.constant);
	glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.linear"), spotLight.linear);
	glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.quadratic"), spotLight.quadratic);
	
	lightShader.use();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));
	
	TLighting::lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();

		processInput(window);
		view = cam->getView();

		glm::vec3 lightCirculation = glm::vec3(glm::cos(currentFrame), glm::sin(currentFrame), glm::cos(currentFrame) * glm::sin(currentFrame));
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		objectShader.use();
		
		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glUniform3fv(glGetUniformLocation(objectShader.ID, "camPos"), 1, glm::value_ptr(cam->getPos()));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "spotLight.position"), 1, glm::value_ptr(cam->getPos()));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "spotLight.direction"), 1, glm::value_ptr(cam->getFront()));

		for (size_t i = 0; i < objects.size(); i++) {
			const auto & obj = objects[i];
			glm::mat4 model = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), obj.position), obj.scale), glm::radians(obj.rotateAngle), obj.rotateAxis);
			
			glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			for (size_t i = 0; i < pointLights.size(); i++) {
				const auto & light = pointLights[i];
				glm::vec3 lightPos = (float)std::pow(-1, i) * lightCirculation + objects[i].position;
				glUniform3fv(glGetUniformLocation(objectShader.ID, std::string("pointLights[" + std::to_string(i) + "].diffuse").c_str()), 1, glm::value_ptr(light.diffuse));
				glUniform3fv(glGetUniformLocation(objectShader.ID, std::string("pointLights[" + std::to_string(i) + "].specular").c_str()), 1, glm::value_ptr(light.specular));
				glUniform3fv(glGetUniformLocation(objectShader.ID, std::string("pointLights[" + std::to_string(i) + "].ambient").c_str()), 1, glm::value_ptr(light.ambient));
				glUniform3fv(glGetUniformLocation(objectShader.ID, std::string("pointLights[" + std::to_string(i) + "].position").c_str()), 1, glm::value_ptr(lightPos));
				glUniform1f(glGetUniformLocation(objectShader.ID, std::string("pointLights[" + std::to_string(i) + "].constant").c_str()), light.constant);
				glUniform1f(glGetUniformLocation(objectShader.ID, std::string("pointLights[" + std::to_string(i) + "].linear").c_str()), light.linear);
				glUniform1f(glGetUniformLocation(objectShader.ID, std::string("pointLights[" + std::to_string(i) + "].quadratic").c_str()), light.quadratic);
			}

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(lightVao);
		lightShader.use();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		for (size_t i = 0; i < pointLights.size(); i++) {
			glm::mat4 model = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), (float)std::pow(-1, i) * lightCirculation + objects[i].position), glm::vec3(0.3f,0.3f,0.3f)), currentFrame, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();

		lastFrame = currentFrame;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &lightVao);
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &diffuseMap);
	glDeleteTextures(1, &specularMap);

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