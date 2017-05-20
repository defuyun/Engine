#include "action.h"

void doge::action::addObj(const std::shared_ptr<doge::object> & obj) {
	this->boundObjs.insert(obj);
}

std::unordered_set<std::shared_ptr<doge::object>> & doge::action::getBoundObjs() {
	return this->boundObjs;
}
