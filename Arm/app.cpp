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
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	DirectionLight directionLight(DIRECTIONAL, 
		vec3(0.2,0.3,0.3),  // ambient
		vec3(50.0f,50.3f,50.3f), // diffuse
		vec3(0.2,0.2,0.2), // specular
		vec3(0.0f,-1.0f,1.0f) // direction
	);

	std::vector<Light *> lights;
	lights.push_back(&directionLight);

	Shader objectShader("shaders/object.vert", "shaders/object.frag");
	Shader quadShader("shaders/quad.vert", "shaders/quad.frag");
	Shader normalShader("shaders/normal.vert", "shaders/normal.frag", "shaders/normal.geom");

	Model box("resources/box/box.vertices");
	Model plane("resources/box/plane.vertices");
	Model cottage("resources/cottage/vertices/cottage.vertices");
	
	lightEngine->createLightUBO(lights);
	engine->createMatrixUBO();
	engine->createSceneFBO();

	auto render = [&cottage, &plane, &box, engine](const Shader & shader) {
		shader.use();
		shader.setBool("useBlinn", engine->useBlinn);
		shader.setBool("useNormalMap", engine->useNormalMap);
		shader.setBool("useDepthMap", engine->useDepthMap);
		shader.setFloat("heightScale", engine->heightScale);

		float blinnNormalizer = 1 + (float)engine->useBlinn;

		if (engine->renderingShadow)
			glCullFace(GL_FRONT);
		shader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(0.5,0.5,0.5)));
		shader.setFloat("material.shininess", 4.0f * blinnNormalizer);
		shader.setFloat("scale", 0.05f);
		cottage.draw(shader);

		shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -5.0f)));
		shader.setFloat("material.shininess", 64.0f * blinnNormalizer);
		shader.setFloat("scale", 1.0f);
		box.draw(shader);
		glCullFace(GL_BACK);

		shader.setMat4("model", glm::mat4(1.0f));
		shader.setFloat("material.shininess", 1.0f * blinnNormalizer);
		shader.setFloat("scale", 1.0f);
		plane.draw(shader);
	};

	objectShader.use();
	objectShader.setVec3("lightDir",-directionLight.direction);

	engine->lastFrame = (float)glfwGetTime();
	while (!glfwWindowShouldClose(engine->window)) {
		engine->currentFrame = (float)glfwGetTime();
		engine->pollEvent();

		engine->updateView();

		engine->renderToFrame(engine->sceneFBO, [&objectShader, &normalShader, &render, engine]() {
			engine->beginRender(engine->sceneFBO);

			render(objectShader);
			if (engine->displayNormal)
				render(normalShader);

			engine->endRender();
		});

		engine->beginRender(0);
		engine->renderFrameToScreen(engine->sceneTexture, glm::mat4(1.0f), quadShader);
		engine->endRender();

		glfwSwapBuffers(engine->window);
		engine->lastFrame = engine->currentFrame;
	}

	glDeleteFramebuffers(1, &engine->sceneFBO);
	glfwTerminate();
}