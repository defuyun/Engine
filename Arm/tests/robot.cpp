#include "robot.h"
#include "shader.h"
#include "robot.h"
#include "stb_image.h"
#include "logger.h"
#include "camera.h"
#include "scene.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>

namespace TRobot {
	void processInput(GLFWwindow *window);
	void mouseCallback(GLFWwindow * window, double xpos, double ypos);
	
	bool keyPress[256] = { false };

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;
	float delta = 0.0f;

	Camera * cam = nullptr;
	Robot * robot = nullptr;

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
}

Robot::Robot(const std::shared_ptr<Object> & root) {
	this->root = this->constructHierarchy(root);
}

std::string getNormalizeName(const std::string & name) {
	return name.substr(0, name.find_last_of('_'));
}

std::shared_ptr<Object> Robot::constructHierarchy(const std::shared_ptr<Object> & root) {
	std::shared_ptr<Object> base = root->children[0];
	auto ring = root->children[1];
	base->addChild(ring);

	std::shared_ptr<Object> curArm = nullptr;
	std::shared_ptr<Object> curBlock = nullptr;
	std::shared_ptr<Object> curRotateCtrl = nullptr;
	std::shared_ptr<Object> curClutch = nullptr;

	for(auto & obj : root->children) {
		obj->name = getNormalizeName(obj->name);
		if (obj->name.substr(0, 3) == "Arm") {
			int number = obj->name[3] - '0' - 1;

			if (obj->name.size() == 4) {
				ring->addChild(obj);
				curArm = obj;
				this->arms[number] = curArm;
				continue;
			}

			auto postfix = obj->name.substr(5, obj->name.size() - 5);

			if (postfix == "Block") {
				curArm->addChild(obj);
				curBlock = obj;
			}
			else if (postfix == "RotationController"){
				curBlock->addChild(obj);
				curRotateCtrl = obj;
				this->rotateUnit[number] = obj;
				this->rotateUnit[number]->model.rotate.axis = glm::vec3(0.0f, 0.0f, 1.0f);
			}
			// ClutchLeft || ClutchRight
			else if (postfix.substr(0, 6) == "Clutch" && postfix.size() <= 12) {
				curRotateCtrl->addChild(obj);
				curClutch = obj;
			}
			else {
				curClutch->addChild(obj);
				std::string posStr = postfix.substr(6, 4);
				if (posStr == "Left") {
					this->clutchUnit[number].left = obj;
				}
				else {
					this->clutchUnit[number].right = obj;
				}
			}
		}
	}

	return base;
}

void Robot::moveArm(float displacement) {
	for (int i = 0; i < TOTAL_ARMS; i++) {
		if (this->active[i])
			this->arms[i]->model.translate += glm::vec3(0.0f, 0.0f, displacement);
	}
}

void Robot::rotateClutch(float angle) {
	for (int i = 0; i < TOTAL_ARMS; i++) {
		if (this->active[i])
			this->rotateUnit[i]->model.rotate.angle += angle;
	}
}

void Robot::moveClutch(float displacement) {
	for (int i = 0; i < TOTAL_ARMS; i++) {
		if (this->active[i]) {
			this->clutchUnit[i].left->model.translate += glm::vec3(displacement, 0.0f, 0.0f);
			this->clutchUnit[i].right->model.translate += glm::vec3(-displacement, 0.0f, 0.0f);
		}
	}
}

void Robot::toggleArm(int id) {
	if (id < 0 || id >= TOTAL_ARMS) return;
	this->active[id] = !this->active[id]; 
}

void Robot::draw(const Shader & shader) const {
	this->root->draw(shader);
}

void run() {
	using namespace TRobot;
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


	glm::vec3 camPos = glm::vec3(0.0f, 5.0f, 7.0f);
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)(800) / 640, 0.1f, 100.0f);
	glm::mat4 view;

	cam = new Camera(camPos);

	Shader objectShader("shaders/robot.vert", "shaders/robot.frag");
	Scene object("resources/robot/robot.obj");

	Robot temp(object.objects[0]);
	robot = &temp;

	// setting constant shader uniforms for object
	objectShader.use();
	{
		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));
		glUniform1f(glGetUniformLocation(objectShader.ID, "material.shininess"), 16);
		glUniform3fv(glGetUniformLocation(objectShader.ID, "material.diffuse"), 1, glm::value_ptr(glm::vec3(0.3f, 0.7f, 1.0f)));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "material.specular"), 1, glm::value_ptr(glm::vec3(0.3f, 0.4f, 0.7f)));
	}

	lastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		currentFrame = (float)glfwGetTime();
		delta = currentFrame - lastFrame;
		processInput(window);
		view = cam->getView();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		objectShader.use();

		glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3fv(glGetUniformLocation(objectShader.ID, "camPos"), 1, glm::value_ptr(cam->getPos()));

		robot->draw(objectShader);

		glfwSwapBuffers(window);
		glfwPollEvents();

		lastFrame = currentFrame;
	}
	glfwTerminate();
}

void TRobot::processInput(GLFWwindow *window)
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
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
		robot->moveArm(0.1f * delta);
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
		robot->moveArm(-0.1f * delta);
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		robot->rotateClutch(1 * delta);
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		robot->rotateClutch(-1 * delta);
	if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
		robot->moveClutch(0.05f * delta);
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS)
		robot->moveClutch(-0.05f * delta);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		keyPress[GLFW_KEY_1] = true;
	if (keyPress[GLFW_KEY_1] && glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		keyPress[GLFW_KEY_1] = false;
		robot->toggleArm(0);
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		keyPress[GLFW_KEY_2] = true;
	if (keyPress[GLFW_KEY_2] && glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		keyPress[GLFW_KEY_2] = false;
		robot->toggleArm(1);
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		keyPress[GLFW_KEY_3] = true;
	if (keyPress[GLFW_KEY_3] && glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
		keyPress[GLFW_KEY_3] = false;
		robot->toggleArm(2);
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		keyPress[GLFW_KEY_4] = true;
	if (keyPress[GLFW_KEY_4] && glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
		keyPress[GLFW_KEY_4] = false;
		robot->toggleArm(3);
	}
}

void TRobot::mouseCallback(GLFWwindow * window, double xpos, double ypos) {
	cam->processMouseMovement(xpos, ypos, currentFrame - lastFrame);
}