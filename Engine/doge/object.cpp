#include "object.h"

doge::baseObject::baseObject(GLuint a) : va(a) {}

void doge::baseObject::bind() const {
	if (this->seperator.empty()) {
		std::runtime_error("[OBJ] you haven't specified a seperator for the base class, call obj->setSeperator()\n");
	}

	if (this->locs.empty()) {
		std::runtime_error("[OBJ] you haven't specified the loc mapping for the base class, call obj->setLocs()\n");
	}

	if (this->seperator.size() != this->locs.size()) {
		std::runtime_error("[OBJ] seperator does not match loc size\n");
	}

	this->bind(this->seperator, this->locs);
}

GLuint doge::baseObject::getVertexArrayObject() const {
	return va;
}

void doge::baseObject::setMesh(const GLfloat * const mesh, int size) {
	this->mesh = std::vector<GLfloat>(mesh, mesh + size);
	this->totalMeshSize = size;
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->mesh.size(), &this->mesh[0], GL_STATIC_DRAW); 
}

void doge::baseObject::setVerticeStart(int start) {
	verticeStart = start;
}

void doge::baseObject::setVerticeCount(int count) {
	verticeCount = count;
}

void doge::baseObject::setElementInterval(int start, int end) {
	elementStart = start;
	elementEnd = end;
}

void doge::baseObject::setElement(const GLshort * const elem, int size) {
	this->elements = std::vector<GLshort>(elem, elem + size);
	this->totalElementSize = size;
	this->hasElement = true;
	glGenBuffers(1, &ve);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ve);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * this->elements.size(), &this->elements[0], GL_STATIC_DRAW);
}

void doge::baseObject::shareMesh(const std::shared_ptr<doge::baseObject> & other) {
	this->vb = other->vb;
	this->ve = other->ve;
	this->verticeStart = other->verticeStart;
	this->verticeCount = other->verticeCount;
	this->totalMeshSize = other->totalMeshSize;
	this->totalElementSize = other->totalElementSize;
	this->hasElement = other->hasElement;
	this->elementStart= other->elementStart;
	this->elementEnd = other->elementEnd;
}

void doge::baseObject::setShaderID(GLuint id) {
	this->pid = id;
}

void doge::baseObject::setSeperator(const std::vector<std::pair<int, int>> & sep) {
	this->seperator = sep;
}

void doge::baseObject::setLocs(const std::vector<std::string> & locs) {
	this->locs = locs;
}

void doge::baseObject::setDefaultSims(const std::vector<doge::siw> & sims) {
	this->defaultSims.clear();
	this->addSim(sims);
}

void doge::baseObject::setStride(int stride) {
	this->stride = stride;
}

void doge::baseObject::addSim(const siw & si) {
	this->defaultSims.emplace(si.loc, si);
}

void doge::baseObject::addSim(const std::vector<siw> & sims) {
	for (auto & si : sims) {
		this->addSim(si);
	}
}

void doge::baseObject::removeSim(const std::string & loc) {
	this->defaultSims.erase(loc);
}

void doge::baseObject::removeSim(const siw & loc) {
	this->defaultSims.erase(loc.loc);
}

void doge::baseObject::bind(const std::vector<std::pair<int, int>> & sep, const std::vector<std::string> & locs) const {
	glBindVertexArray(va); {
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		for (size_t i = 0; i < sep.size(); i++) {
			GLuint loc = glGetAttribLocation(pid, locs[i].c_str());
			glVertexAttribPointer(loc, sep[i].second - sep[i].first, GL_FLOAT, GL_FALSE, 
				this->stride * sizeof(GLfloat), (void *)(sep[i].first * sizeof(GLfloat)));
			glEnableVertexAttribArray(loc);
		}
	} glBindVertexArray(0);
}

doge::object * doge::object::setFront(const glm::vec3 & front) {
	this->front = front;
	return this;
}

doge::object * doge::object::setPos(const glm::vec3 & pos) {
	this->pos = pos;
	return this;
}

doge::object * doge::object::setUp(const glm::vec3 & up) {
	this->up = up;
	return this;
}

doge::object * doge::object::setRotate(const glm::vec3 & rotate) {
	this->rotate = rotate;
	return this;
}

doge::object * doge::object::setScale(const glm::vec3 & scale) {
	this->scale = scale;
	return this;
}

doge::object * doge::object::setAngle(GLfloat angle) {
	this->angle = angle;
	return this;
}

doge::object * doge::object::setDraw(bool draw) {
	this->_draw = draw;
	return this;
}

doge::object * doge::object::setAlive(bool alive) {
	this->_alive = alive;
	return this;
}

glm::vec3 doge::object::getFront() const {
	return front;
}

glm::vec3 doge::object::getPos() const {
	return pos;
}

glm::vec3 doge::object::getUp() const {
	return up;
}

glm::vec3 doge::object::getRotate() const {
	return rotate;
}

glm::vec3 doge::object::getScale() const {
	return scale;
}

GLfloat doge::object::getAngle() const {
	return angle;
}

bool doge::object::getDraw() const {
	return _draw;
}

bool doge::object::getAlive() const {
	return _alive;
}

glm::mat4 doge::object::getModel() const {
	return glm::scale(glm::rotate(glm::translate(glm::mat4(), pos), angle, rotate), scale);
}

void doge::object::addSim(const doge::siw & si, doge::object::opt option) {
	if (option == doge::object::EXC) {
		excludeSims.emplace(si.loc, si);
	} else {
		overwriteSims.emplace(si.loc, si);
	}
}

void doge::object::addSim(const std::vector<doge::siw> & sims, doge::object::opt option) {
	for (auto & si : sims) {
		this->addSim(si, option);
	}
}

void doge::object::removeSim(const std::string & loc, doge::object::opt option) {
	if (option == doge::object::EXC) {
		excludeSims.erase(loc);
	} else {
		overwriteSims.erase(loc);
	}
}

void doge::object::removeSim(const doge::siw & loc, doge::object::opt option) {
	this->removeSim(loc.loc, option);
}

void doge::object::useSim(const siw & si) const {
	this->_sim->use(this->_bo->pid, si);
}

void doge::object::draw(const std::shared_ptr<camera> & cam) const {
	if (!this->getAlive() || !this->getDraw()) {
		return;
	}

	GLuint pid = this->_bo->pid;
	glBindVertexArray(this->_bo->va); {
		glUseProgram(pid);
		glBindBuffer(GL_ARRAY_BUFFER, this->_bo->vb);
		if (this->_bo->hasElement) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_bo->ve);
		}
			
		for (auto & p : this->_bo->defaultSims) {
			if (overwriteSims.find(p.first) == overwriteSims.end() &&
				excludeSims.find(p.first) == excludeSims.end()) {
				this->useSim(p.second);
			}
		}

		for (auto & p : overwriteSims) {
			this->useSim(p.second);
		}
		
		glUniformMatrix4fv(glGetUniformLocation(pid, "model"), 1, GL_FALSE, glm::value_ptr(getModel()));
		glUniformMatrix4fv(glGetUniformLocation(pid, "project"), 1, GL_FALSE, glm::value_ptr(cam->getProject()));
		glUniformMatrix4fv(glGetUniformLocation(pid, "view"), 1, GL_FALSE, glm::value_ptr(cam->getView()));

		if (this->_bo->hasElement) {
			glDrawElements(GL_TRIANGLES, this->_bo->elementEnd - this->_bo->elementStart,
				GL_UNSIGNED_SHORT, (void *)(sizeof(GLshort) * this->_bo->elementStart));
		} else {
			glDrawArrays(GL_TRIANGLES, this->_bo->verticeStart, this->_bo->verticeCount);
		}
		glUseProgram(0);
	} glBindVertexArray(0);
}

doge::camera::camera(std::unique_ptr<shaderIndexManager> & sim, const std::shared_ptr<baseObject> & bo) : object(sim,bo) {
	this->setDraw(false);
	this->setAlive(false);
}

doge::camera * doge::camera::setFov(GLfloat fov) {
	this->fov = fov;
	return this;
}

doge::camera * doge::camera::setAspect(GLfloat aspect) {
	this->aspect = aspect;
	return this;
}

doge::camera * doge::camera::setNear(GLfloat near) {
	this->near = near;
	return this;
}

doge::camera * doge::camera::setFar(GLfloat far) {
	this->far = far;
	return this;
}

glm::mat4 doge::camera::getProject() const{
	return glm::perspective(fov, aspect, near, far);
}

glm::mat4 doge::camera::getView() const {
	return glm::lookAt(getPos(), getPos() + getFront(), getUp());
}