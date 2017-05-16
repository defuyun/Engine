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

void doge::Engine::addTexture(const std::string & file, const siw & si) {
	GLuint newt;
	glGenTextures(1, &newt);
	_sim->add(sdw<GLint>(si.loc, si.cls, newt));
	int tw, th;
	unsigned char * image = SOIL_load_image(file.c_str(), &tw, &th, 0, SOIL_LOAD_RGB);

	glActiveTexture(GL_TEXTURE0 + _sim->getTextureCount());
	glBindTexture(GL_TEXTURE_2D, newt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	_sim->addTextureCount();

	SOIL_free_image_data(image);
}

GLuint doge::Engine::createProgram(const std::string & name, const std::vector<shfile> & files) {
	return _shader->createProgram(name, files);
}

GLuint doge::Engine::getPid(const std::string & name) const {
	return _shader->getPid(name);
}
