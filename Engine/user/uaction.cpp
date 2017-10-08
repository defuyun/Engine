#include "uaction.h"

namespace doge
{
	void lamp_move::execute(std::unique_ptr<mainEngineControl> & mec, std::shared_ptr<doge::object> & obj)
	{
		if (mec->isKeyPressed(_front)) {
			obj->setPos(obj->getPos() + obj->getFront() * this->_moveSpeed);
		} else if (mec->isKeyPressed(_back)) {
			obj->setPos(obj->getPos() - obj->getFront() * this->_moveSpeed);
		} else if (mec->isKeyPressed(_right)) {
			obj->setPos(obj->getPos() + glm::normalize(glm::cross(obj->getFront(), obj->getUp())) * this->_moveSpeed);
		} else if (mec->isKeyPressed(_left)){
			obj->setPos(obj->getPos() - glm::normalize(glm::cross(obj->getFront(), obj->getUp())) * this->_moveSpeed);
		} else if (mec->isKeyPressed(_up)){
			obj->setPos(obj->getPos() + obj->getUp() * this->_moveSpeed);
		} else {
			obj->setPos(obj->getPos() - obj->getUp() * this->_moveSpeed);
		}
		auto position = siw{ "lightPos_", "default", doge::type::VEC3 };
		getSimInstance()->set(position, obj->getPos());
	}

	bool lamp_move::check(std::unique_ptr<mainEngineControl> & mec, const std::shared_ptr<doge::object> & obj) const
	{
		return mec->isKeyPressed(_front) || mec->isKeyPressed(_back) || mec->isKeyPressed(_left) || mec->isKeyPressed(_right)
			||  mec->isKeyPressed(_up) || mec->isKeyPressed(_down);
	}
}