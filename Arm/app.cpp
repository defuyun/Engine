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
	
	glm::vec3 camPos = glm::vec3(0.0f, 5.0f, -2.0f);
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
		vec3(0.0f, 0.0f, -1.0f), // direction,
		30.0f,
		45.0f
	);

	std::vector<Light *> lights;
	
	lights.push_back(&directionLight);
	lights.push_back(&pointLight);
	lights.push_back(&spotLight);

	lightEngine->createLightUBO(lights);

	Shader quadShader("shaders/quad.vert", "shaders/quad.frag");
	GLuint quadTexId = loadTextureFromFile("resources/box", "grass.png");

	engine->lastFrame = (float)glfwGetTime();
	while (!glfwWindowShouldClose(engine->window)) {
		engine->currentFrame = (float)glfwGetTime();
		engine->pollEvent();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		
		quadShader.use();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, quadTexId);
		glUniform1i(glGetUniformLocation(quadShader.ID, "quadTex"), (GLint)1);

		engine->renderQuad();

		glfwSwapBuffers(engine->window);
		engine->lastFrame = engine->currentFrame;
	}

	glfwTerminate();
}