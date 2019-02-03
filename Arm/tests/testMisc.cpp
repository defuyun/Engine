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
		glm::vec3 specular;
		glm::vec3 ambient;
	};

	struct PointLight : public Light {
		PointLight(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient, float constant, float linear, float quadratic, const glm::vec3 & position) :
			Light(diffuse, specular, ambient), constant(constant), linear(linear), quadratic(quadratic) {}

		float constant;
		float linear;
		float quadratic;
		glm::vec3 position;
	};

	struct DirectionLight : public Light {
		DirectionLight(const glm::vec3 & diffuse, const glm::vec3 & specular, const glm::vec3 & ambient, const glm::vec3 & direction) : Light(diffuse, specular, ambient), direction(direction) {}
		glm::vec3 direction;
	};

	struct SpotLight : public PointLight {
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
}


void testMisc() {
	GLFWwindow * window = nullptr;

	const int WIN_WIDTH = 1024;
	const int WIN_HEIGHT = 640;

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
		glfwSetCursorPosCallback(window, TMisc::mouseCallback);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			Logger->log("[ENG] Can't initialize glew\n");
		}

		glEnable(GL_DEPTH_TEST);
	}

	Shader objectShader("shaders/object.vert", "shaders/object.frag");
	Shader planeShader("shaders/plane.vert", "shaders/plane.frag");

	glm::vec3 camPos = glm::vec3(0.0f, 2.0f, -2.0f);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)(WIN_WIDTH) / WIN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view;

	TMisc::cam = new Camera(camPos);

	TMisc::DirectionLight directionLight(
		glm::vec3(0.3f, 0.3f, 0.5f), // diffuse
		glm::vec3(0.2f, 0.2f, 0.5f), // specular
		glm::vec3(0.2f, 0.2f, 0.5f), // ambient
		glm::vec3(0.0f, -1.0f, -1.0f) // direction
	);

	Model box("resources/box/box.vertices");
	
	objectShader.use();
	{
		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));
		glUniform1f(glGetUniformLocation(objectShader.ID, "material.shininess"), 16.0f);
		glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.diffuse"), 1, glm::value_ptr(directionLight.diffuse));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.specular"), 1, glm::value_ptr(directionLight.specular));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.ambient"), 1, glm::value_ptr(directionLight.ambient));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "directionLight.direction"), 1, glm::value_ptr(directionLight.direction));
	}

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint fbTex, fbSten;
	glGenTextures(1, &fbTex);
	glBindTexture(GL_TEXTURE_2D, fbTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTex, 0);

#ifdef TEXTUREBUFFER
	glGenTextures(1, &fbSten);
	glBindTexture(GL_TEXTURE_2D, fbSten);

	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT, 0, GL_DEPTH_STENCIL,GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbSten, 0);
#else
	glGenRenderbuffers(1, &fbSten);
	glBindRenderbuffer(GL_RENDERBUFFER, fbSten);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbSten);
#endif

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error, framebuffer incomplete\n";
	}
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat plane[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	GLuint planeVBO, planeVAO;

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *) (sizeof(GLfloat) * 2));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	TMisc::lastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		TMisc::currentFrame = (float)glfwGetTime();

		TMisc::processInput(window);
		view = TMisc::cam->getView();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 b1Pos = glm::translate(model, glm::vec3(3.0, 0.0f, 3.0f));

		objectShader.use();
		{
			glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniform3fv(glGetUniformLocation(objectShader.ID, "camPos"), 1, glm::value_ptr(TMisc::cam->getPos()));
		}

		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		box.draw(objectShader);

		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(b1Pos));
		box.draw(objectShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

#define FBO

#ifdef FBO
		planeShader.use();
		glActiveTexture(GL_TEXTURE1);
		GLuint loc = glGetUniformLocation(planeShader.ID, "tex");
		glUniform1i(loc, (GLint)1);
		glBindTexture(GL_TEXTURE_2D,fbTex);
			
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glActiveTexture(0);
#endif // FBO

		glfwSwapBuffers(window);
		glfwPollEvents();

		TMisc::lastFrame = TMisc::currentFrame;
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
