#include "app.h"

#include "components/camera.h"
#include "components/shader.h"
#include "components/model.h"
#include "components/engine.h"
#include "components/light.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>

void run() {
	using glm::vec3;

	Engine e;
	Engine * engine = &e;
	
	glm::vec3 camPos = glm::vec3(0.0f, 4.0f, -4.0f);
	Camera c(camPos);
	Camera * cam = &c;

	LightEngine lc;
	LightEngine * lightEngine = &lc;

	engine->cam = cam;

	const int WIN_WIDTH = 1280;
	const int WIN_HEIGHT = 720;

	engine->init(WIN_WIDTH, WIN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	DirectionLight directionLight(DIRECTIONAL, 
		vec3(0.2,0.3,0.3),  // ambient
		vec3(0.2,0.3,0.3), // diffuse
		vec3(0.5,0.5,0.5), // specular
		vec3(0.0f,-1.0f,-1.0f) // direction
	);

	PointLight pointLight(POINT,
		vec3(0.4,0.5,0.5),  // ambient
		vec3(0.2,0.3,0.3), // diffuse
		vec3(0.5,0.5,0.5), // specular
		vec3(3.0f,3.0f, 3.0f), // position
		0.2f,
		0.3f,
		0.5f
	);

	SpotLight spotLight(SPOT,
		vec3(0.2,0.3,0.3),  // ambient
		vec3(0.2,0.3,0.3), // diffuse
		vec3(0.5,0.5,0.5), // specular
		vec3(0.0f,0.0f, 0.0f), // position
		0.2f,
		0.3f,
		0.5f,
		vec3(0.0f, 0.0f, 0.0f), // direction,
		30.0f,
		45.0f
	);

	std::vector<Light *> lights;
	
	lights.push_back(&directionLight);
	lights.push_back(&pointLight);
	lights.push_back(&spotLight);

	Shader objectShader("shaders/object.vert", "shaders/object.frag");
	
	objectShader.use(); {
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
		glm::mat4 rotate = glm::rotate(scale, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		objectShader.setMat4("model", rotate);
	}

	Model nanosuit("resources/nanosuit/nanosuit.obj");

	lightEngine->createLightUBO(lights);
	engine->createMatrixUBO();

	engine->lastFrame = (float)glfwGetTime();
	while (!glfwWindowShouldClose(engine->window)) {
		engine->currentFrame = (float)glfwGetTime();
		engine->pollEvent();

		engine->updateView();
		lightEngine->updateLightParameter(SPOT, 0, offsetof(SpotLight, position), cam->getPos());
		lightEngine->updateLightParameter(SPOT, 0, offsetof(SpotLight, direction), (cam->getFront() - cam->getPos()));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		
		nanosuit.draw(objectShader);

		glfwSwapBuffers(engine->window);
		engine->lastFrame = engine->currentFrame;
	}

	glfwTerminate();
}