#include "engine.h"

std::unique_ptr<doge::Engine> doge::engine = std::make_unique<doge::Engine>();

doge::Engine::Engine() {
	if (!glfwInit()) {
		std::runtime_error("[ENG] Can't init glfw\n");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->_shader = std::make_unique<shader>();
	this->_sim = std::make_unique<shaderIndexManager>();
}

void doge::Engine::createWindow(const std::string & name, int w, int h) {
	this->_window = std::make_unique<display>(name, w, h);
	glfwMakeContextCurrent(this->_window->window);

	glfwSetInputMode(this->_window->window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(this->_window->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::runtime_error("[ENG] Can't initialize glew\n");
	}
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

bool doge::Engine::isRunning() {
	return !glfwWindowShouldClose(this->_window->window);
}

GLuint doge::Engine::createProgram(const std::string & name, const std::vector<shfile> & files) {
	return _shader->createProgram(name, files);
}

GLuint doge::Engine::getPid(const std::string & name) const {
	return _shader->getPid(name);
}
