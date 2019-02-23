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
		vec3(0.2,0.3,0.3), // diffuse
		vec3(0.2,0.2,0.2), // specular
		vec3(0.0f,-1.0f,1.0f) // direction
	);

	PointLight pointLight(POINT,
		vec3(0.4,0.5,0.5),  // ambient
		vec3(0.7,0.7,0.7), // diffuse
		vec3(0.5,0.5,0.5), // specular
		vec3(6.0f,4.0f, 6.0f), // position
		0.1f,
		0.1f,
		0.1f
	);

	SpotLight spotLight(SPOT,
		vec3(0.2,0.3,0.3),  // ambient
		vec3(0.6,0.6,0.6), // diffuse
		vec3(0.7,0.7,0.7), // specular
		vec3(0.0f,0.0f, 0.0f), // position
		0.3f,
		0.1f,
		0.2f,
		vec3(0.0f, 0.0f, 0.0f), // direction,
		glm::cos(glm::radians(30.0f)),
		glm::cos(glm::radians(45.0f))
	);

	std::vector<Light *> lights;
	
	lights.push_back(&directionLight);
	lights.push_back(&pointLight);
	lights.push_back(&spotLight);

	Shader objectShader("shaders/object.vert", "shaders/object.frag");
	Shader quadShader("shaders/quad.vert", "shaders/quad.frag");
	Shader normalShader("shaders/normal.vert", "shaders/normal.frag", "shaders/normal.geom");
	Shader skyboxShader("shaders/skybox.vert", "shaders/shadowskybox.frag");
	Shader shadowShader("shaders/shadow.vert", "shaders/shadow.frag", "shaders/shadow.geom");

	Model box("resources/box/box.vertices");
	Model plane("resources/box/plane.vertices");
	Model light("resources/sphere/sphere.obj");
	Model cottage("resources/cottage/vertices/cottage.vertices");
	
	lightEngine->createLightUBO(lights);
	engine->createMatrixUBO();
	engine->createSceneFBO();
	engine->createShadowSkyboxFBO();

	glm::vec3 lightPos = pointLight.position;

	auto render = [&cottage, &plane, &light, &box, &lightPos, engine](const Shader & shader) {
		shader.use();
		shader.setBool("blinn", engine->useBlinn);

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

		shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.2f,0.2f,0.2f)));
		shader.setFloat("material.shininess", 32.0f * blinnNormalizer);
		shader.setFloat("scale", 0.05f);
		light.draw(shader);

		shader.setMat4("model", glm::mat4(1.0f));
		shader.setFloat("material.shininess", 1.0f * blinnNormalizer);
		shader.setFloat("scale", 1.0f);
		plane.draw(shader);
	};

	objectShader.use();
	objectShader.setFloat("lightNear", engine->SHADOW_NEAR);
	objectShader.setFloat("lightFar", engine->SHADOW_FAR);

	engine->lastFrame = (float)glfwGetTime();
	while (!glfwWindowShouldClose(engine->window)) {
		engine->currentFrame = (float)glfwGetTime();
		engine->pollEvent();

		engine->updateView();

		glm::vec3 center(0.0f, 3.0f, 0.0f);
		glm::vec3 lightCenterDir = center - lightPos;
		glm::vec3 lightMoveDir = glm::cross(lightCenterDir, glm::vec3(0.0f, 1.0f, 0.0f));

		lightPos += lightMoveDir * 0.2f * (engine->currentFrame - engine->lastFrame);

		lightEngine->updateLightParameter(SPOT, 0, offsetof(SpotLight, position), cam->getPos());
		lightEngine->updateLightParameter(SPOT, 0, offsetof(SpotLight, direction), cam->getFront());
		lightEngine->updateLightParameter(POINT, 0, offsetof(PointLight, position), lightPos);

		engine->createShadowMap(lightPos, shadowShader, render);

		engine->renderToFrame(engine->shadowSkyboxFBO, [&skyboxShader, engine]() {
			engine->beginRender(engine->shadowSkyboxFBO);
			engine->bindShadowMap(skyboxShader);
			engine->renderCube();
			engine->endRender();
		});

		engine->renderToFrame(engine->sceneFBO, [&lightPos, &objectShader, &normalShader, &render, engine]() {
			engine->beginRender(engine->sceneFBO);
			engine->bindShadowMap(objectShader);
			objectShader.use();
			objectShader.setVec3("lightPos", lightPos);
			objectShader.setBool("useNormalMap", engine->useNormalMap);

			render(objectShader);
			if (engine->displayNormal)
				render(normalShader);

			engine->endRender();
		});

		engine->beginRender(0);
		engine->renderFrameToScreen(engine->sceneTexture, glm::mat4(1.0f), quadShader);
		engine->renderFrameToScreen(engine->shadowSkyboxTexture, glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.5f,0.5f, 0.0f)), glm::vec3(0.4,0.4,0.4)), quadShader);
		engine->endRender();

		glfwSwapBuffers(engine->window);
		engine->lastFrame = engine->currentFrame;
	}

	glDeleteFramebuffers(1, &engine->sceneFBO);
	glfwTerminate();
}