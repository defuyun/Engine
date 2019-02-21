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
		vec3(2.0f,3.0f, 2.0f), // position
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
	Shader shadowShader("shaders/shadow.vert", "shaders/shadow.frag", "shaders/shadow.geom");
	Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");

	Model nanosuit("resources/nanosuit/nanosuit.obj");
	Model plane("resources/box/plane.vertices");
	Model light("resources/sphere/sphere.obj");

	lightEngine->createLightUBO(lights);
	engine->createMatrixUBO();
	engine->createSceneFBO();

	glm::vec3 lightPos = pointLight.position;

	auto render = [&nanosuit, &plane, &light, &lightPos, engine](const Shader & shader) {
		shader.use();
		shader.setBool("blinn", engine->useBlinn);

		float blinnNormalizer = 1 + (int)engine->useBlinn;

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
		glm::mat4 rotate = glm::rotate(scale, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("model", rotate);
		shader.setFloat("material.shininess", 16.0f * blinnNormalizer);
		shader.setFloat("scale", 0.05f);
		nanosuit.draw(shader);

		shader.setMat4("model", glm::mat4(1.0f));
		shader.setFloat("material.shininess", 1.0f * blinnNormalizer);
		shader.setFloat("scale", 1.0f);
		plane.draw(shader);

		shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.2f,0.2f,0.2f)));
		shader.setFloat("material.shininess", 32.0f * blinnNormalizer);
		shader.setFloat("scale", 0.05f);
		light.draw(shader);
	};

	const int SHADOW_WIDTH = 1280, SHADOW_HEIGHT = 1280;
	GLuint depthMap, depthFBO;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);

	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float shadowFar = 15.0f;
	glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / SHADOW_HEIGHT, 0.1f, shadowFar);
	
	shadowShader.use();
	shadowShader.setMat4("shadowProjection", shadowProjection);
	shadowShader.setFloat("near", 0.1f);
	shadowShader.setFloat("far", shadowFar);

	objectShader.use();
	objectShader.setFloat("lightNear", 0.1f);
	objectShader.setFloat("lightFar", shadowFar);

	glm::vec3 shadowDirections[6] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
	};

	glm::vec3 shadowUpDirections[6] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
	};

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

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadowShader.use();
		shadowShader.setVec3("lightPos", lightPos);

		for (int i = 0; i < 6; ++i)
			shadowShader.setMat4("shadowView[" + std::to_string(i) + ']', glm::lookAt(lightPos, lightPos + shadowDirections[i], shadowUpDirections[i]));

		render(shadowShader);

		glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, engine->sceneFBO);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);

		objectShader.use();
		objectShader.setInt("depthMap", (GLint)5);
		render(objectShader);
		if (engine->displayNormal)
			render(normalShader);
	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, engine->sceneTexture);
		quadShader.use();
		quadShader.setInt("tex", (GLint)1);
		quadShader.setBool("gamma", Model::useGamma);

		engine->renderQuad();
		glActiveTexture(GL_TEXTURE0);

		glfwSwapBuffers(engine->window);
		engine->lastFrame = engine->currentFrame;
	}

	glDeleteFramebuffers(1, &engine->sceneFBO);
	glfwTerminate();
}