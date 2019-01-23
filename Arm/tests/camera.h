#pragma once

#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

const float YAW = -90.0f;
const float PITCH = -45.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 25.0f;

enum CameraDirection {
	front,
	back,
	left,
	right
};

class Camera {
private:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;

	float yaw;
	float pitch;
	
	float speed;
	float sensitivity;

	double lastX;
	double lastY;

	bool firstMouse = true;
public:
	explicit Camera(const glm::vec3 & pos,float yaw = YAW, float pitch = PITCH, float speed = SPEED, float sensitivity = SENSITIVITY) :
		pos(pos), yaw(yaw), pitch(pitch), speed(speed), sensitivity(sensitivity){
		update();
	}

	void processKeyPress(CameraDirection dir, float delta);
	void processMouseMovement(double xpos, double ypos, float delta);

	const glm::vec3 & getPos() const;
	const glm::vec3 & getFront() const;
	glm::mat4 getView() const;
private:
	void update();
};
