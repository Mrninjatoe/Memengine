#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	Camera();
	virtual ~Camera();
	void update(float deltaTime);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	glm::vec3 pos;
	glm::vec3 cameraTarget;
	glm::vec3 direction;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::ivec2 mousePos = { 0,0 };
	
	float fov = 110.f;
	float zNear = 0.1f;
	float zFar = 100.f;
	float pitch = 0.f;
	float yaw = 0.f;
	float speedMultipler = 1.0f;
	
	glm::quat orientation;

	bool pressedShift = false;
	bool moveUp = false;
	bool moveDown = false;
	bool moveForward = false;
	bool moveBack = false;
	bool moveRight = false;
	bool moveLeft = false;
	bool enableMouse = true;

	float timeCounter;
};


//float radius = 2.f;
//float camX = sin(_timeCounter) * radius;
//float camZ = cos(_timeCounter) * radius;
//
//_pos.x = camX;
//_pos.z = camZ;
//
//_timeCounter += 1 * deltaTime;