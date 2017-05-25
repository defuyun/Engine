#include "control.h"

doge::mainEngineControl::mainEngineControl(int keySize, int mouseSize) 
	: _keyPressed(keySize, false), _mouseClicked(mouseSize, false) {
	_cursorX = _cursorY = _scrollOffsetX = _scrollOffsetY = 0.0;
	_sensitivity = 0.5f;
	_firstRunFlag = true;
}

void doge::mainEngineControl::setSensitivity(double sensitivity) {
	this->_sensitivity = sensitivity;
}

void doge::mainEngineControl::setPitchInterval(double pitchMin, double pitchMax) {
	this->_pitchMin = pitchMin;
	this->_pitchMax = pitchMax;
}
void doge::mainEngineControl::setYawInterval(double yawMin, double yawMax) {
	this->_yawMin = yawMin;
	this->_yawMax = yawMax;
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

	double preCursorX = _cursorX;
	double preCursorY = _cursorY;
	
	_cursorX = cursorX;
	_cursorY = cursorY;

	_pitch += (preCursorY - _cursorY) * _sensitivity;
	_yaw += (_cursorX - preCursorX) * _sensitivity;

	if (_pitch > _pitchMax) {
		_pitch = _pitchMax;
	}
	
	if (_pitch < _pitchMin) {
		_pitch = _pitchMin;
	}

	if (_yaw > _yawMax) {
		_yaw = _yawMax;
	}

	if (_yaw < _yawMin) {
		_yaw = _yawMin;
	}
}

void doge::mainEngineControl::setScroll(double scrollX, double scrollY) {
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

std::pair<double, double> doge::mainEngineControl::getScroll() const {
	return{ this->_scrollOffsetX, this->_scrollOffsetY };
}

double doge::mainEngineControl::getPitch() const {
	return _pitch;
}

double doge::mainEngineControl::getYaw() const {
	return _yaw;
}