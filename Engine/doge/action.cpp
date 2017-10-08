#include "action.h"

void doge::action::addObj(const std::shared_ptr<doge::object> & obj) {
	this->boundObjs.insert(obj);
}

std::unordered_set<std::shared_ptr<doge::object>> & doge::action::getBoundObjs() {
	return this->boundObjs;
}

void doge::default_move::execute(std::unique_ptr<doge::mainEngineControl> & mec, std::shared_ptr<doge::object> & obj) {
	if (mec->isKeyPressed(_front)) {
		obj->setPos(obj->getPos() + obj->getFront() * this->_moveSpeed);
	} else if (mec->isKeyPressed(_back)) {
		obj->setPos(obj->getPos() - obj->getFront() * this->_moveSpeed);
	} else if (mec->isKeyPressed(_right)) {
		obj->setPos(obj->getPos() + glm::normalize(glm::cross(obj->getFront(), obj->getUp())) * this->_moveSpeed);
	} else {
		obj->setPos(obj->getPos() - glm::normalize(glm::cross(obj->getFront(), obj->getUp())) * this->_moveSpeed);
	}
	auto cameraPos = siw{ "cameraPos_", "default", doge::type::VEC3 };
	getSimInstance()->set(cameraPos, obj->getPos());
}

bool doge::default_move::check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const {
	return mec->isKeyPressed(_front) || mec->isKeyPressed(_back) || mec->isKeyPressed(_left) || mec->isKeyPressed(_right);
}

void doge::default_move::setMoveSpeed(float moveSpeed) {
	this->_moveSpeed = moveSpeed;
}

void doge::default_move::setKeyBinding(int front, int back, int left, int right) {
	this->_front = front;
	this->_back = back;
	this->_left = left;
	this->_right = right;
}

void doge::default_lookAround::execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj) {
	glm::vec3 front = obj->getFront();

	front.y = (GLfloat)glm::sin(glm::radians(mec->getPitch()));
	front.x = (GLfloat)glm::cos(glm::radians(mec->getPitch())) * (GLfloat)glm::cos(glm::radians(mec->getYaw()));
	front.z = (GLfloat)glm::cos(glm::radians(mec->getPitch())) * (GLfloat)glm::sin(glm::radians(mec->getYaw()));

	obj->setFront(glm::normalize(front));
}

bool doge::default_lookAround::check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const {
	return true;
}
