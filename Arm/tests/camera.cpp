#include "camera.h"

const glm::vec3 & Camera::getPos() const {
	return this->pos;
}

void Camera::processKeyPress(CameraDirection dir, float delta) {
	if (dir == CameraDirection::front)
		this->pos += this->front * this->speed * delta;
	else if (dir == CameraDirection::back)
		this->pos -= this->front * this->speed * delta;
	else if (dir == CameraDirection::left)
		this->pos += glm::normalize(glm::cross(this->up, this->front)) * this->speed * delta;
	else if (dir == CameraDirection::right)
		this->pos -= glm::normalize(glm::cross(this->up, this->front)) * this->speed * delta;
}

void Camera::processMouseMovement(double xpos, double ypos, float delta) {
	if (this->firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		return;
	}
	float displaceX = ((float)xpos - lastX) * sensitivity * delta;
	float displaceY = (lastY - (float)ypos) * sensitivity * delta;

	this->yaw += displaceX;
	this->pitch += displaceY;

	if (pitch < -89.0f)
		pitch = -89.0f;
	
	if (pitch > 89.0f)
		pitch = 89.0f;
		
	update();
	
	lastX = xpos;
	lastY = ypos;
}

glm::mat4 Camera::getView() const {
	return glm::lookAt(this->pos, this->pos + this->front, this->up);
}

void Camera::update() {
	this->front.x = glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
	this->front.y = glm::sin(glm::radians(this->pitch));
	this->front.z = glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
	
	if (abs(front.x) < 10e-5)
		front.x = 0;

	if (abs(front.y) < 10e-5)
		front.y = 0;
	
	if (abs(front.z) < 10e-5)
		front.z = 0;

	this->front = glm::normalize(this->front);
	glm::vec3 right = glm::normalize(glm::cross(this->front, glm::vec3(0.0f, 1.0f, 0.0f)));
	this->up = glm::normalize(glm::cross(right, this->front));
}
