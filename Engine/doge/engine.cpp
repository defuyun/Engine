#include "engine.h"

std::unique_ptr<doge::Engine> doge::engine = std::make_unique<doge::Engine>();

void doge::Engine::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode) {
	engine->_mec->setKeyPressed(key, action);
}

void doge::Engine::cursor_callback(GLFWwindow * window, double posx, double posy) {
	engine->_mec->setCursorPos(posx, posy);
}

void doge::Engine::scroll_callback(GLFWwindow * window, double offx, double offy) {
	engine->_mec->setScroll(offx, offy);
}

doge::Engine::Engine() {
	if (!glfwInit()) {
		std::runtime_error("[ENG] Can't init glfw\n");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->_shader = std::make_unique<shader>();
	this->_sim = std::make_unique<shaderIndexManager>();
	this->_mec = std::make_unique<mainEngineControl>();
	this->_mec->setPitchInterval(-89.0, 89.0);
	this->_mec->setYawInterval(-360.0, 360.0);
	this->_mec->setSensitivity(0.5);
}

void doge::Engine::createWindow(const std::string & name, int w, int h) {
	this->_window = std::make_unique<display>(name, w, h);
	glfwMakeContextCurrent(this->_window->window);

	glfwSetInputMode(this->_window->window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(this->_window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	
	glfwSetKeyCallback(this->_window->window, key_callback);
	glfwSetCursorPosCallback(this->_window->window, cursor_callback);
	glfwSetScrollCallback(this->_window->window, scroll_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::runtime_error("[ENG] Can't initialize glew\n");
	}
	
	auto it = this->createBaseObject(_camva, "baseCamera");
	this->_cam = std::make_shared<camera>(this->_sim, it);
	this->_cam->setFront(glm::vec3(0.0f, 0.0f, -1.0f))->setPos(glm::vec3(0.0f, 0.0f, 3.0f))->setUp(glm::vec3(0.0f,1.0f,0.0f));


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

bool doge::Engine::isRunning() {
	return !glfwWindowShouldClose(this->_window->window);
}

void doge::Engine::addTexture(const std::string & file, const siw & si) {
	GLuint newt;
	glGenTextures(1, &newt);
	_sim->add(sdw<GLint>(si.loc, si.cls, _sim->getTextureCount()));
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

GLuint doge::Engine::createVertexArrayObject() {
	GLuint va;
	glGenVertexArrays(1, &va);
	_vao.insert(va);
	return va;
}

std::shared_ptr<doge::baseObject> doge::Engine::createBaseObject(GLuint va, const std::string & name) {
	if (_bobjs.find(name) != _bobjs.end()) {
		std::runtime_error("[ENG] creating duplicate base object: " + name + '\n');
	}

	std::shared_ptr<doge::baseObject> newbobj(new baseObject(va));
	_bobjs.emplace(name, newbobj);
	return newbobj;
}

std::shared_ptr<doge::object> doge::Engine::createObject(const std::shared_ptr<baseObject> & bobj) {
	std::shared_ptr<doge::object> newobj(new object(this->_sim, bobj));
	this->_objs.insert(newobj);
	return newobj;
}

std::shared_ptr<doge::baseObject> & doge::Engine::getBaseObject(const std::string & name) {
	auto it = this->_bobjs.find(name);
	if (it == this->_bobjs.end()) {
		std::runtime_error("[ENG] trying to get non-exist base object: " + name + '\n');
	}

	return it->second;
}

std::shared_ptr<doge::camera> & doge::Engine::getCamera() {
	return this->_cam;
}

void doge::Engine::update() {
	glfwPollEvents();
	for (auto & actions : this->_act) {
		// can't use a reference because values in set are read only, because if you modify the
		// value the indexing has to change
		for (auto object : actions->getBoundObjs()) {
			if (actions->check(_mec, object)) {
				actions->execute(_mec, object);
			}
		}
	}
}

void doge::Engine::draw() const {
	glfwSwapBuffers(this->_window->window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto & it : this->_objs) {
		it->draw(this->_cam);
	}
}

void doge::Engine::setControlPitchInterval(double pitchMin, double pitchMax) {
	this->_mec->setPitchInterval(pitchMin,pitchMax);
}

void doge::Engine::setControlYawInterval(double yawMin, double yawMax) {
	this->_mec->setYawInterval(yawMin, yawMax);
}

void doge::Engine::setControlSensitivity(double sensitivity) {
	this->_mec->setSensitivity(sensitivity);
}
