#pragma once

#include "camera.h"
#include "shader.h"

#include <functional>
#include <GL/glew.h>
#include <GLFW\glfw3.h>

const int MATRIX_BINDING = 1;
const int CAMERAPOS_BINDING = 5;

class Engine {
public:
	void renderQuad();
	void renderCube();

	Camera * cam = nullptr;
	GLFWwindow * window = nullptr;

	float currentFrame = 0.0f;
	float lastFrame = 0.0f;

	GLuint sceneFBO = 0;
	GLuint sceneTexture = 0;

	GLuint shadowSkyboxFBO = 0;
	GLuint shadowSkyboxTexture = 0;

	GLuint shadowMapFBO = 0;
	GLuint shadowMap = 0;

	void init(int width, int height);
	void pollEvent();

	void createMatrixUBO();
	void updateView();
	void createSceneFBO();
	void createShadowSkyboxFBO();
	void createShadowMap(const glm::vec3 & lightPos, const Shader & shadowShader, const std::function<void(const Shader &)> & render);
	void bindShadowMap(const Shader & shader) const;
	
	void renderFrameToScreen(GLuint textureId, const glm::mat4 & model,const Shader & quadShader);
	void renderSceneToFrame(const Shader & objectShader, const std::function<void()> & render);

	void renderToFrame(GLuint fbo, const std::function<void()> & render) const;

	void beginRender(GLuint fbo) const;
	void endRender() const;

	bool displayNormal = false;
	bool useNormalMap = true;
	bool useBlinn = true;

	bool renderingShadow = false;

	void createFBO(int width, int height, GLuint * fbo, GLuint * texture);

	const float SHADOW_NEAR = 0.1f;
	const float SHADOW_FAR = 15.0f;
private:
	GLuint quadVAO = 0;
	GLuint cubeVAO = 0;
	GLuint matrixUBO = 0;
	GLuint camPosUBO = 0;

	const int SHADOW_WIDTH = 1280;
	const int SHADOW_HEIGHT = 1280;

	const std::string shadowMapLocation = "shadowMap";

	bool keyPress[256] = { false };

	void processInput(GLFWwindow *window);
	void processMousePos(GLFWwindow * window);

	void initShadowMap(const Shader & shadowShader);
};
