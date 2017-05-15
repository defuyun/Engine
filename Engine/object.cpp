#include "object.h"

doge::baseObject::baseObject(GLuint a) : va(a) {}

void doge::baseObject::setMesh(const GLfloat * const mesh, int size) {
	this->mesh = std::vector<GLfloat>(mesh, mesh + size);
	glGenBuffers(1, &vb);
	glBindVertexArray(va); {
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->mesh.size(), &this->mesh[0], GL_STATIC_DRAW); 
	} glBindVertexArray(0);
}

void doge::baseObject::setInterval(int start, int end) {
	verticeStart = start;
	verticeEnd = end;
}

void doge::baseObject::setElement(const GLshort * const elem, int size) {
	this->elements = std::vector<GLshort>(elem, elem + size);
	glGenBuffers(1, &ve);
	glBindVertexArray(va); {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ve);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * this->elements.size()
			, &this->elements[0], GL_STATIC_DRAW);
	} glBindVertexArray(0);
	this->hasElement = true;
}

void doge::baseObject::shareMesh(const std::shared_ptr<doge::baseObject> & other) {
	this->va = other->va;
	this->vb = other->vb;
	this->ve = other->ve;
	this->pid = other->pid;
	this->hasElement = other->hasElement;
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

void setDefaultSims(const std::vector<std::string> & sim) {

}