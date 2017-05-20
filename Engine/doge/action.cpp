#include "action.h"

void doge::action::addObj(const std::shared_ptr<doge::object> & obj) {
	this->boundObjs.insert(obj);
}

std::unordered_set<std::shared_ptr<doge::object>> & doge::action::getBoundObjs() {
	return this->boundObjs;
}

void doge::move::execute(std::unique_ptr<doge::mainEngineControl> & mec, std::shared_ptr<doge::object> & obj) {
	if (mec->isKeyPressed(_front)) {
		obj->setPos(obj->getPos() + obj->getFront() * this->_moveSpeed);
	} else if (mec->isKeyPressed(_back)) {
		obj->setPos(obj->getPos() - obj->getFront() * this->_moveSpeed);
	} else if (mec->isKeyPressed(_right)) {
		obj->setPos(obj->getPos() + glm::normalize(glm::cross(obj->getFront(), obj->getUp()) * this->_moveSpeed));
	} else {
		obj->setPos(obj->getPos() - glm::normalize(glm::cross(obj->getFront(), obj->getUp()) * this->_moveSpeed));
	}
}

bool doge::move::check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const {
	return mec->isKeyPressed(_front) || mec->isKeyPressed(_back) || mec->isKeyPressed(_left) || mec->isKeyPressed(_right);
}

void doge::move::setMoveSpeed(float moveSpeed) {
	this->_moveSpeed = moveSpeed;
}

void doge::move::setKeyBinding(int front, int back, int left, int right) {
	this->_front = front;
	this->_back = back;
	this->_left = left;
	this->_right = right;
}
