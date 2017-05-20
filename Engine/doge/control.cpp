#include "control.h"

doge::mainEngineControl::mainEngineControl(int keySize = 1024, int mouseSize = 8) 
	: _keyPressed(keySize, false), _mouseClicked(mouseSize, false) {
	_cursorX = _cursorY = _preCursorX = _preCursorY = _scrollOffsetX = _scrollOffsetY = 0.0;
	_firstRunFlag = true;
}

void doge::mainEngineControl::setKeyPressed(int key, int action) {
	this->_keyPressed[key] = action != GLFW_RELEASE;
}

void doge::mainEngineControl::setMouseClicked(int button, int action) {
	this->_mouseClicked[button] = action != GLFW_RELEASE;
}

void doge::mainEngineControl::setCursorPos(double cursorX, double cursorY) {
	if (_firstRunFlag) {
		this->_cursorX = cursorX;
		this->_cursorY = cursorY;
		this->_firstRunFlag = false;
	}

	_preCursorX = cursorX;
	_preCursorY = cursorY;
	
	_cursorX = cursorX;
	_cursorY = cursorY;
}

void doge::mainEngineControl::setScroll(double scrollY, double scrollX = 0) {
	this->_scrollOffsetX = scrollX;
	this->_scrollOffsetY = scrollY;
}

bool doge::mainEngineControl::isKeyPressed(int key) const {
	return this->_keyPressed[key];
}

bool doge::mainEngineControl::isMouseClicked(int button) const {
	return this->_mouseClicked[button];
}

std::pair<double, double> doge::mainEngineControl::getCursorPos() const {
	return{ this->_cursorX, this->_cursorY };
}
std::pair<double, double> doge::mainEngineControl::getPrevCursor() const {	
	return{ this->_preCursorX, this->_preCursorY };
}
std::pair<double, double> doge::mainEngineControl::getScroll() const {
	return{ this->_scrollOffsetX, this->_scrollOffsetY };
}

double doge::mainEngineControl::getPitch(double sensitivity) const {

}

double doge::mainEngineControl::getYaw(double sensitivity) const {

}
