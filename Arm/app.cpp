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

	engine->cam = cam;

	const int WIN_WIDTH = 1280;
	const int WIN_HEIGHT = 720;

	engine->init(WIN_WIDTH, WIN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	Shader gbufferShader("shaders/gbuffer.vert", "shaders/gbuffer.frag");
	Shader quadShader("shaders/quad.vert", "shaders/quad.frag");
	Shader defferedShader("shaders/deferred.vert", "shaders/deferred.frag");

	Model box("resources/box/box.vertices");
	Model plane("resources/box/plane.vertices");
	Model well("resources/well/collada/well_blender.dae");

	LightEngine l({
		SimpleLight(glm::vec3(5.0f, 1.0f, -5.0f), glm::vec3(10.0f, 10.0f, 10.0f)),
		SimpleLight(glm::vec3(-5.0f, 1.0f, 5.0f), glm::vec3(10.0f, 10.0f, 10.0f)),
		SimpleLight(glm::vec3(-5.0f, 1.0f, -5.0f), glm::vec3(10.0f, 10.0f, 10.0f)),
		SimpleLight(glm::vec3(5.0f, 1.0f, 5.0f), glm::vec3(10.0f, 10.0f, 10.0f)),
		SimpleLight(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f)),
	});

	LightEngine * lights = &l;

	engine->createMatrixUBO();
	engine->createSceneFBO();
	engine->createGBuffer();
	engine->createDefferedFBO();

	auto render = [&well, &plane, &box, engine](const Shader & shader) {
		shader.use();
		float blinnNormalizer = 1 + (float)engine->useBlinn;

		glDisable(GL_CULL_FACE);
		shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f)), glm::vec3(0.2,0.2,0.2)));
		shader.setFloat("scale", 0.05f);
		well.draw(shader);
		glEnable(GL_CULL_FACE);

		shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -5.0f)));
		shader.setFloat("scale", 1.0f);
		box.draw(shader);

		shader.setMat4("model", glm::mat4(1.0f));
		shader.setFloat("scale", 1.0f);
		plane.draw(shader);
	};

	engine->lastFrame = (float)glfwGetTime();
	while (!glfwWindowShouldClose(engine->window)) {
		engine->currentFrame = (float)glfwGetTime();
		engine->pollEvent();

		engine->updateView();

		engine->renderToFrame(engine->gbufferFBO, [&gbufferShader, &render, engine]() {
			render(gbufferShader);
		});

		engine->renderToFrame(engine->defferedFBO, [&defferedShader, engine]() {
			engine->bindGBuffer(defferedShader);
			engine->renderQuad();
		});

		engine->beginRender(0);
		engine->renderFrameToScreen(engine->defferedTexture, FrameOrient{ SCREEN_POS::CENTER, SCREEN_SIZE::L}, quadShader);
		engine->endRender();

		glfwSwapBuffers(engine->window);
		engine->lastFrame = engine->currentFrame;
	}

	glDeleteFramebuffers(1, &engine->sceneFBO);
	glfwTerminate();
}